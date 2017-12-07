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
static void speakerOutputTimerHandler(unsigned long data);
static void speakerControlTimerHandler(unsigned long data);
static void sonarReadTimerHandler(unsigned long data);
static unsigned int us_to_ft(int data);

static ssize_t gpio_read(struct file* filp, char* buf, size_t count, loff_t* f_pos);
static ssize_t gpio_write( struct file *filp, const char __user *buff, unsigned long len, void *data);

/* GPIO definitions */
#define SONAR 101
#define SONAR_READ 17
#define GPIO_MAJOR 61
#define SPEAKER_OUT 113

/* General Defines */
#define LOW 0
#define HIGH 1

/* Timer Interval Defines */
#define SONAR_READ_PERIOD_NORMAL 10000 /* msecs */
#define SONAR_READ_PERIOD_FAST 100   /* msecs */
#define SONAR_READ_PERIOD_DISPLAY 60000 /* msecs, halts sampling if display is on */
#define SONAR_READ_HALT 10 /* usecs */
#define TIMER_PERIOD 60000 /* msecs */

/* Soanr Distance Defines */
#define FAST_SAMPLE_LIMIT 100	/* How many times to sample in fast trigger period */
#define DIST_THRESHOLD 2	/* Distance threshold in feet */
#define US_TO_FEET 1764
#define TRIGGER_THRESHOLD 10	/* How many times threshold must be triggered to cause display to be active */

/* Timer variables */
static struct timer_list* sonarReadTimer;
static struct timer_list* speakerOutputTimer;
static struct timer_list* speakerControlTimer;

/* Speaker Variables */
static int speakerEn = 0;
static int count = 0;

/* Sonar Variables */
static struct timeval* prevTime;
static int sampleFast = 0;	 /* State variable that causes faster sampling rate */
static int fastSampleCount = 0;	 /* How many samples have occured during fast sample rate */
static int fastTriggerCount = 0; /* How many times a person has been detected during the increased sampling rate */
static int displayState = 0;
static int throttle = 0;

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
    do_gettimeofday(prevTime);
  }
  else {
    struct timeval curTime;
    do_gettimeofday(&curTime);
    int usDist = timeval_compare(&curTime, prevTime); // Distance in microseconds
    /* int ftDist = us_to_ft(usDist); */
    int ftDist = usDist / US_TO_FEET;
    if (ftDist < DIST_THRESHOLD && !sampleFast) 
      sampleFast = 1;
    else if (ftDist < DIST_THRESHOLD && sampleFast) {
      if (fastTriggerCount++ > TRIGGER_THRESHOLD) {
	displayState = 1;
	speakerEn = 1;
      }
    }
    printk(KERN_ALERT "\n\nusDist: %d \t ftDist: %d \t sampleFast: %d \t fastTriggerCount: %d \t displayState: %d \t speakerEn: %d \r", usDist, ftDist, sampleFast, fastTriggerCount, displayState, speakerEn);
  }

  return IRQ_HANDLED;
}

/*****************************************************************
 * Convert us to feet for PWM
 * 2 LSB equal fractional value
 * Rest of bits correspond to feet
 *****************************************************************/
unsigned int us_to_ft(int us) {
  /* unsigned int ft = us / US_TO_FEET; // Find feet */
  /* unsigned int in = us % US_TO_FEET;  // Find inches */
  
  /* Translate fractional componenet to fixed-point */
  /* unsigned int tenths = in / 10; */
  /* unsigned int hund = in % 10; */
  /* hund /= 10; */
  return 0;
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
	setup_timer(speakerOutputTimer, speakerOutputTimerHandler, 0);
	mod_timer(speakerOutputTimer, jiffies + msecs_to_jiffies(TIMER_PERIOD));

	/* Timer controlling note being outputted */
	speakerControlTimer = (struct timer_list*) kmalloc(sizeof(struct timer_list), GFP_KERNEL);
	if (!speakerControlTimer) {
	  printk(KERN_ALERT "Unable to assign Speaker Control timer, exiting\n");
	  my_cleanup_module();
	  return -ENOMEM;
	}
	setup_timer(speakerControlTimer, speakerControlTimerHandler, 0);
	mod_timer(speakerControlTimer, jiffies + msecs_to_jiffies(TIMER_PERIOD));


	/* Sonar Read Timer, triggers read */
	sonarReadTimer = (struct timer_list*) kmalloc(sizeof(struct timer_list), GFP_KERNEL);
	if (!sonarReadTimer) {
	  printk(KERN_ALERT "Unable to assign timer, exiting\n");
	  my_cleanup_module();
	  return -ENOMEM;
	}
	setup_timer(sonarReadTimer, sonarReadTimerHandler, 0);
	mod_timer(sonarReadTimer, jiffies + msecs_to_jiffies(SONAR_READ_PERIOD_NORMAL));

	
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

  /* Motion detected read rate control */
  if (speakerEn && displayState) {
    pxa_gpio_set_value(SONAR_READ, LOW);

    /* Decrease sample rate if motion is detected */
    if (displayState && speakerEn && !throttle) {
      throttle = 1;
      sampleFast = 0;
      fastSampleCount = 0;
      mod_timer(sonarReadTimer, jiffies + msecs_to_jiffies(SONAR_READ_PERIOD_DISPLAY));
    }

    /* Return sample rate to normal after throttle period */
    else if (displayState && speakerEn && throttle) {
      displayState = 0;
      speakerEn = 0;
      throttle = 0;
      fastTriggerCount = 0;
      mod_timer(sonarReadTimer, jiffies + msecs_to_jiffies(SONAR_READ_PERIOD_NORMAL));
    }
  }

  /* Motion not detected read rate control */
  else if (pxa_gpio_get_value(SONAR_READ)) {
    pxa_gpio_set_value(SONAR_READ, LOW);

    /* Change sampling rate if motion is detected */
    if (sampleFast && fastSampleCount++ < FAST_SAMPLE_LIMIT) 
      mod_timer(sonarReadTimer, jiffies + msecs_to_jiffies(SONAR_READ_PERIOD_FAST));

    /* Return sample rate to normal if motion detection threshold is not exceeded */
    else {
      mod_timer(sonarReadTimer, jiffies + msecs_to_jiffies(SONAR_READ_PERIOD_NORMAL));
      fastSampleCount = 0;
      sampleFast = 0;
      fastTriggerCount = 0;
    }
  }

  
  else {
    pxa_gpio_set_value(SONAR_READ, HIGH);
    mod_timer(sonarReadTimer, jiffies + usecs_to_jiffies(SONAR_READ_HALT));
  }
}

static void speakerOutputTimerHandler (unsigned long data) {
  mod_timer(speakerOutputTimer, jiffies + msecs_to_jiffies(TIMER_PERIOD));
}

static void speakerControlTimerHandler (unsigned long data) {
  mod_timer(speakerControlTimer, jiffies + msecs_to_jiffies(TIMER_PERIOD));
}



/***********************************************************************************
 * Timer Callback for count
 **********************************************************************************/

static void my_cleanup_module(void)
{
  unregister_chrdev(GPIO_MAJOR, "mygpio");
  free_irq(IRQ_GPIO(SONAR), NULL);

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
