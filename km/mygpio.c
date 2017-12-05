#ifndef __KERNEL__
#define __KERNEL__
#endif

#ifndef MODULE
#define MODULE
#endif

#include <linux/module.h>
#include <linux/interrupt.h>
#include <asm/arch/pxa-regs.h>
#include <asm-arm/arch/hardware.h>
#include <asm-arm/arch/gpio.h>
/* #include <stdint.h> */

/* Device drivers from timer labs */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/io.h>
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/time.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/jiffies.h> /* jiffies */
#include <asm/system.h> /* cli(), *_flags */
#include <asm/uaccess.h> /* copy_from/to_user */

/* Function Headers*/ 
static void my_cleanup_module(void);
static void slowTimerHandler(unsigned long data);
static void sonarTimerHandler(unsigned long data);
static void sonarReadTimerHandler(unsigned long data);

static ssize_t gpio_read(struct file* filp, char* buf, size_t count, loff_t* f_pos);
static ssize_t gpio_write( struct file *filp, const char __user *buff, unsigned long len, void *data);

/* GPIO definitions */
#define TIMER_OUT 113
#define SONAR 101
#define SONAR_READ 17
#define GPIO_MAJOR 61


/* General Defines */
#define LOW 0
#define HIGH 1
#define SONAR_PERIOD 1 //usecs
#define SONAR_READ_PERIOD 5000  //msecs
#define SONAR_READ_HALT 10 // usecs
#define TIMER_PERIOD 60000 //msecs

/* Timer variables */
static struct timer_list* sonarReadTimer;
static struct timer_list* speakerOutputTimer;
static struct timer_list* speakerControlTimer;

/* Speaker Variables */
static int speakerEn;

/* Sonar Variables */
static struct timeval* prevTime;

/* Global Variables */
char* bufKern;

/* General FOPS */
struct file_operations gpio_fops = {
	 write: gpio_write, 
	 read: gpio_read
};

/*****************************************************************
 * IRQ Handlers 
 *****************************************************************/
irqreturn_t gpio_irq101_rising(int irq, void *dev_id, struct pt_regs *regs)
{
  
  if (pxa_gpio_get_value(SONAR)) {
    printk(KERN_ALERT "HIGH Interrupt \n");
    /* do_gettimeofday(prevTime); */
    printk(KERN_ALERT "Time 1 in s: %lu \t Time 1 in us: %lu \n", (long int) (prevTime->tv_sec), (long int) prevTime->tv_usec);    
    printk(KERN_ALERT "\n\n\n");

  }
  else {
    struct timeval curTime;
    do_gettimeofday(&curTime);
    printk(KERN_ALERT "Low \n");
    printk(KERN_ALERT "Time 1 in s: %lu \n", (long int) (prevTime->tv_sec));    
    printk(KERN_ALERT "Time 1 in s: %lu \n", (long int) (curTime.tv_sec));    

    printk(KERN_ALERT "Time 1 in us: %lu \n", (long int) (prevTime->tv_usec));
    printk(KERN_ALERT "Time 2 in us: %lu \n", (long int) (curTime.tv_usec));
    printk(KERN_ALERT "Difference in time  in us: %d \n", timeval_compare(&curTime, prevTime));
    printk(KERN_ALERT "\n\n\n");

  }

  return IRQ_HANDLED;
}

/* Instantiate all timers needed for program */

static int startTimers(void) {
	/* Timer controlling pulse width of output signal */
	speakerOutputTimer = (struct timer_list*) kmalloc(sizeof(struct timer_list), GFP_KERNEL);
	if (!speakerOutputTimer) {
	  printk(KERN_ALERT "Unable to assign Speaker Output timer, exiting\n");
	  my_cleanup_module();
	  return -ENOMEM;
	}
	setup_timer(speakerTimer, speakerOutputTimerHandler, 0);
	mod_timer(speakerTimer, jiffies + msecs_to_jiffies(TIMER_PERIOD));

	/* Timer controlling note being outputted */
	speakerControlTimer = (struct timer_list*) kmalloc(sizeof(struct timer_list), GFP_KERNEL);
	if (!speakerTimer) {
	  printk(KERN_ALERT "Unable to assign Speaker Control timer, exiting\n");
	  my_cleanup_module();
	  return -ENOMEM;
	}
	setup_timer(speakerTimer, speakerControlTimerHandler, 0);
	mod_timer(speakerTimer, jiffies + msecs_to_jiffies(TIMER_PERIOD));


	/* Sonar Read Timer, triggers read */
	sonarReadTimer = (struct timer_list*) kmalloc(sizeof(struct timer_list), GFP_KERNEL);
	if (!sonarReadTimer) {
	  printk(KERN_ALERT "Unable to assign timer, exiting\n");
	  my_cleanup_module();
	  return -ENOMEM;
	}
	setup_timer(sonarReadTimer, sonarReadTimerHandler, 0);
	mod_timer(sonarReadTimer, jiffies + msecs_to_jiffies(SONAR_READ_PERIOD));

	
	return 1;
}

/**********************************************************************
 * Kernel Functions 
 *********************************************************************/
static int my_init_module(void)
{
    int result;
    int irq;
    /* Registering device */
	result = register_chrdev(GPIO_MAJOR, "mygpio", &gpio_fops);
	if (result < 0)
	  {
	    printk(KERN_ALERT
		   "gpioTimer: cannot obtain major number %d\n", GPIO_MAJOR);
	    return result;
	  }
	

	/* Set buttons as inputs */
	gpio_direction_input(SONAR);
	gpio_direction_output(SONAR_READ, LOW);
	gpio_direction_output(SPEAKER_OUT, LOW);

	/* Intialize interrupts */
	irq = IRQ_GPIO(SONAR);
	if (request_irq(irq, (void*) &gpio_irq101_rising, SA_INTERRUPT | SA_TRIGGER_RISING | SA_TRIGGER_FALLING,
				"sonarRising", NULL) != 0 ) {
                printk ( "irq not acquired \n" );
                return -1;
        }else{
                printk ( "irq %d acquired successfully \n", irq );
	}

	/* Intialize Timers */
	if (startTimers() != 1) {
	  return -ENOMEM;
	}

	/* Initialize Global Variables */
	speakerEn = 0;

	/* Declare timespec struct */
	prevTime = (struct timeval*) kmalloc(sizeof(struct timeval), GFP_KERNEL);
	if (!prevTime) {
	  printk(KERN_ALERT "Unable to allocatet timespec struct\n");
	  return -ENOMEM;
	}

	/* Intialize buffer */
	bufKern = (char*) kmalloc(256, GFP_KERNEL);
	if (!bufKern) {
	  printk(KERN_ALERT "Unable to assign timer, exiting\n");
	  my_cleanup_module();
        return -ENOMEM;
	}

	printk("mygpio: Inserting mygpio module\n");
	return 0;
}

/***********************************************************************************
 * Timer for triggering sonar ouput
 **********************************************************************************/
static void sonarReadTimerHandler (unsigned long data) {
  if (pxa_gpio_get_value(SONAR_READ)) {
    pxa_gpio_set_value(SONAR_READ, LOW);
    mod_timer(sonarReadTimer, jiffies + msecs_to_jiffies(SONAR_READ_PERIOD));
  }
  else {
    pxa_gpio_set_value(SONAR_READ, HIGH);
    mod_timer(sonarReadTimer, jiffies + usecs_to_jiffies(SONAR_READ_HALT));


    printk(KERN_ALERT "\n HIGH timer \n");
    do_gettimeofday(prevTime);
    printk(KERN_ALERT "Time 1 in s: %lu \t Time 1 in us: %lu \n", (long int) (prevTime->tv_sec), (long int) prevTime->tv_usec);    
    printk(KERN_ALERT "\n\n\n");

  }
}


/***********************************************************************************
 * Timer Callback for count
 **********************************************************************************/

static void my_cleanup_module(void)
{
  unregister_chrdev(GPIO_MAJOR, "mygpio");
  free_irq(IRQ_GPIO(SONAR), NULL);

  if(slowTimer) kfree(slowTimer);
  if(bufKern)  kfree(bufKern);
}



/***********************************************************************************
 * Takes user input and modifies period 
 **********************************************************************************/
static ssize_t gpio_write( struct file *filp, const char __user *buff, unsigned long len, void *data) {
  return len;
}


/***********************************************************************************
 * Prints relavent program data
 **********************************************************************************/
static ssize_t gpio_read(struct file* filp, char* buf, size_t _count, loff_t* f_pos) {
  int len = 0;
  memset(bufKern, 0, 256);

  /* int unit = timer_period/1000; */
  /* int deci = timer_period - (unit * 1000); */
  
  if (*f_pos > 0) 
    return 0;

  /* /\* Print Data To Buffer *\/ */
  /* len += sprintf((bufKern+len), "CounterValue: %d\n", count); */
  /* len += sprintf((bufKern+len), "CounterPeriod: %d.%d secs\n", unit, deci); */
  /* switch (bLevel) { */
  /* case 1:  */
  /*   len += sprintf((bufKern+len), "Brightness Level: 50%% duty cycle\n"); */
  /*   break; */

  /* case 2:  */
  /*   len += sprintf((bufKern+len), "Brightness Level: 25%% duty cycle\n"); */
  /*   break; */

  /* case 3: */
  /*   len += sprintf((bufKern+len), "Brightness Level: 10%% duty cycle\n"); */
  /*   break; */
  /* } */

  /* if (dir) */
  /*   len += sprintf((bufKern+len), "Dir: %s\n", "UP"); */
  /* else */
  /*   len += sprintf((bufKern+len), "Dir: %s\n", "DOWN"); */

  /* if (!en)  */
  /*   len += sprintf((bufKern+len), "State: %s\n", "RUNNING"); */
  /* else  */
  /*   len += sprintf((bufKern+len), "State: %s\n", "STOPPED"); */

  /* if (copy_to_user(buf, bufKern, len)) return -EFAULT; */

  /* printk(KERN_ALERT "Test\n"); */
  
  *f_pos += len;
  return len;
}

module_init(my_init_module);
module_exit(my_cleanup_module);
