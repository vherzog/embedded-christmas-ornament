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

static ssize_t gpio_read(struct file* filp, char* buf, size_t count, loff_t* f_pos);
static ssize_t gpio_write( struct file *filp, const char __user *buff, unsigned long len, void *data);

/* GPIO definitions */
#define SONAR 101		// I2C-7
#define SONAR_READ 17		// I2C-6
#define GPIO_MAJOR 61
#define SPEAKER_OUT 113

/* General Defines */
#define LOW 0
#define HIGH 1

/* Timer Interval Defines */
/* #define SONAR_READ_PERIOD_NORMAL 10000 /\* msecs *\/ */
#define SONAR_READ_PERIOD_NORMAL 5000 /* msecs */
#define SONAR_READ_PERIOD_FAST 100   /* msecs */
/* #define SONAR_READ_PERIOD_DISPLAY 60000 /\* msecs, halts sampling if display is on *\/ */
#define SONAR_READ_PERIOD_DISPLAY 20000 /* msecs, halts sampling if display is on */
#define SONAR_READ_HALT 10 /* usecs */
#define TIMER_PERIOD 60000 /* msecs */

/* Soanr Distance Defines */
#define FAST_SAMPLE_LIMIT 100	/* How many times to sample in fast trigger period */
#define DIST_THRESHOLD 2	/* Distance threshold in feet */
#define US_TO_FEET 1764
#define TRIGGER_THRESHOLD 9	/* How many times threshold must be triggered to cause display to be active, one less than acutal value because initial trigger must be included. */

/* Timer variables */
static struct timer_list* sonarReadTimer;
static struct timer_list* speakerOutputTimer;
static struct timer_list* speakerControlTimer;

/* Speaker Variables */
static int speakerEn = 0;

/* Sonar Variables */
static int sonarState = 1;
static int prevSonarState = 0;
static struct timeval* prevTime;
static int sampleFast = 0;	 /* State variable that causes faster sampling rate */
static int fastSampleCount = 0;	 /* How many samples have occured during fast sample rate */
static int fastTriggerCount = 0; /* How many times a person has been detected during the increased sampling rate */
static int displayEn = 0;
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
    int ftDist = usDist / US_TO_FEET;
    if (ftDist < DIST_THRESHOLD && !sampleFast) 
      sampleFast = 1;
    else if (ftDist < DIST_THRESHOLD && sampleFast) {
      fastTriggerCount++;
    }
    //printk(KERN_ALERT "\n\nusDist: %d \t ftDist: %d \t sampleFast: %d \t fastTriggerCount: %d \t displayEn: %d \t speakerEn: %d \r", usDist, ftDist, sampleFast, fastTriggerCount, displayEn, speakerEn);
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
 * Change to case statment with more explicitly designed states
 * Case 0: Sampling pin is high
 * Case 1: Sampling at normal rate, no displays enabled, sample pin is low
 * Case 2: Sampling at faster rate, no displays enable, sample pin is low
 * Case 3: Sampling at throttled rate, displays enable, sample pin is low
 **********************************************************************************/
static void sonarReadTimerHandler (unsigned long data) {

 switch (sonarState) {
 case 0:
   pxa_gpio_set_value(SONAR_READ, HIGH);

   /* Return to normal sampling if not triggered */
   if (prevSonarState == 1) {
     if (sampleFast) 
       sonarState = 2;
     else 
       sonarState = 1;
   }

   /* Check if fast sampling limit has been exceeded */
   else if (prevSonarState == 2) {
     if (fastSampleCount < FAST_SAMPLE_LIMIT) {
       if (fastTriggerCount > TRIGGER_THRESHOLD) {
	 sonarState = 3;
	 fastTriggerCount = 0;
	 speakerEn = 1;
	 displayEn = 1;
       }
       else 
	 sonarState = 2;
     }
     else {
       sonarState = 1;
       sampleFast = 0;
       displayEn = 0;
       speakerEn = 0;
     }
   }

   /* Recheck sensor to see if displays need to be left on */
   else if (prevSonarState == 3) {
     sonarState = 2;
   }

   prevSonarState = 0;
   mod_timer(sonarReadTimer, jiffies + usecs_to_jiffies(SONAR_READ_HALT));
   break;

 case 1:
   pxa_gpio_set_value(SONAR_READ, LOW);

   /* Reset variable associated with faster sampling */
   fastSampleCount = 0;
   fastTriggerCount = 0;

   /* Reset Display Variables */
   speakerEn = 0;
   displayEn = 0;

   /* Assign State Values */
   prevSonarState = 1;
   sonarState = 0;
   mod_timer(sonarReadTimer, jiffies + msecs_to_jiffies(SONAR_READ_PERIOD_NORMAL));
   break;

 case 2:
   pxa_gpio_set_value(SONAR_READ, LOW);

   /* Increment sampling count */
   fastSampleCount++;
   
   /* Save current state*/
   sonarState = 0;
   prevSonarState = 2;

   mod_timer(sonarReadTimer, jiffies + msecs_to_jiffies(SONAR_READ_PERIOD_FAST));
   break;

 case 3:
   pxa_gpio_set_value(SONAR_READ, LOW);
   
   /* Enable outputs */
   speakerEn = 1;
   displayEn = 1;
   
   /* Reset values used to determine trigger */
   fastSampleCount = 0;
   fastTriggerCount = 0;

   /* Set State variables */
   sonarState = 0;
   prevSonarState = 3;
   mod_timer(sonarReadTimer, jiffies + msecs_to_jiffies(SONAR_READ_PERIOD_DISPLAY));
   break;

 default: 
   pxa_gpio_set_value(SONAR_READ, LOW);
   fastSampleCount = 0;
   fastTriggerCount = 0;
   sampleFast = 0;
   prevSonarState = 1;
   sonarState = 0;
   mod_timer(sonarReadTimer, jiffies + msecs_to_jiffies(SONAR_READ_PERIOD_NORMAL));
   break;
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
static int readEn = 0;
static ssize_t gpio_read(struct file* filp, char* buf, size_t _count, loff_t* f_pos) {
  int len = 0;
  memset(bufKern, 0, 256);

 // if (*f_pos > 0)
  //  return 0;

  if(readEn) {
  	readEn = 0;
	return 0;
  } else {
	//printk(KERN_ALERT "WERE IN HERE WITH VALUES %d %d\n", displayEn, speakerEn);
	readEn = 1;
  	len += sprintf(bufKern, "%d %d", displayEn, speakerEn);
	if(copy_to_user(buf, bufKern, len)) {
		return -EFAULT;
	}
  	*f_pos += len;
  }
  
  return len;
}

module_init(my_init_module);
module_exit(my_cleanup_module);

//10:B7:F6:06:F9:82 
