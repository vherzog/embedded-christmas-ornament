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
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/jiffies.h> /* jiffies */
#include <asm/system.h> /* cli(), *_flags */
#include <asm/uaccess.h> /* copy_from/to_user */

/* Function Headers*/ 
static void my_cleanup_module(void);
static void timer_handler(unsigned long data);
static void sonarTimerHandler(unsigned long data);
static ssize_t gpio_read(struct file* filp, char* buf, size_t count, loff_t* f_pos);
static ssize_t gpio_write( struct file *filp, const char __user *buff, unsigned long len, void *data);
static int modPWMRegs(int regNUM, int Prescale, int dutyCycle, int periodValue);

/* GPIO definitions */
#define MYGPIO 113
#define HIGH 1
#define LOW 0
#define LED0 16
#define LED1 29
#define LED2 30
#define LED3 31
#define BTN0 17
#define BTN1 101
#define SONAR 101
#define GPIO_MAJOR 61
#define SONAR_PERIOD 1
#define TIMER_PERIOD 60000

/* Global variables */
static int dir, en, irq17, irq101, reset, bLevel;
static int count;
static struct timer_list* ledTimer;
static struct timer_list* sonarTimer; /* Simultaneous timer */
char* bufKern;
static int timer_period;
static int haltSonar;
static int sonarCount;
static int prevSonarRead;
static unsigned long prevJiffies;
static int slowCount;

/* General FOPS */
struct file_operations gpio_fops = {
	 write: gpio_write, 
	 read: gpio_read
};

/* IRQ Handlers */
irqreturn_t gpio_irq17_rising(int irq, void *dev_id, struct pt_regs *regs)
{
  if (pxa_gpio_get_value(SONAR)) {
    prevJiffies = jiffies;
  }
  else {
    if (jiffies-prevJiffies > 10) {
      printk(KERN_ALERT "Jiffie difference: %u\n", jiffies-prevJiffies);
      printk(KERN_ALERT "Jiffie0: %u \t Jiffie1: %u\n", prevJiffies, jiffies);
    }
  }

  /* unsigned long temp; */
  /* if (pxa_gpio_get_value(SONAR)) { */
  /*   prevJiffies = jiffies; */
  /*   printk(KERN_INFO "Rising Edge\n"); */
  /*   prevSonarRead = sonarCount; */
  /* } */
  /* else { */
  /*   printk(KERN_INFO "Interval: %u\n",  (sonarCount - prevSonarRead)); */
  /*   printk(KERN_INFO "Jiffies0: %u\t Jiffies1:%u\n", prevJiffies, jiffies); */
  /* } */
  return IRQ_HANDLED;
}


/* Falling edge interrupt handler */
irqreturn_t gpio_irq17_falling(int irq, void *dev_id, struct pt_regs *regs)
{
  if (jiffies-prevJiffies > 10) {
    printk(KERN_ALERT "Jiffie difference: %u\n", jiffies-prevJiffies);
    printk(KERN_ALERT "Jiffie0: %u \t Jiffie1: %u\n", prevJiffies, jiffies);
  }
  return IRQ_HANDLED;
}

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

	/* Intialize interrupts */
	irq = IRQ_GPIO(SONAR);
	if (request_irq(irq, (void*) &gpio_irq17_rising, SA_INTERRUPT | SA_TRIGGER_RISING | SA_TRIGGER_FALLING,
				"sonarRising", NULL) != 0 ) {
                printk ( "irq not acquired \n" );
                return -1;
        }else{
                printk ( "irq %d acquired successfully \n", irq );
	}

	/* irq = IRQ_GPIO(SONAR); */
	/* if (request_irq(irq, (void*) &gpio_irq17_falling, SA_INTERRUPT | SA_TRIGGER_FALLING, */
	/* 			"sonarFalling", NULL) != 0 ) { */
        /*         printk ( "irq not acquired \n" ); */
        /*         return -1; */
        /* }else{ */
        /*         printk ( "irq %d acquired successfully \n", irq ); */
	/* } */

	/* Initialize global variables */
	count = 15;
	en = 1;
	dir = 1;
	timer_period = 1000;
	bLevel = 1; /* Brightness Level */

	/* Used to determine simultaneous button presses */
	irq17 = 0;
	irq101 = 0;
	reset = 0;

	/* Initialize timers for incrementing count */
	ledTimer = (struct timer_list*) kmalloc(sizeof(struct timer_list), GFP_KERNEL);
	if (!ledTimer) {
	  printk(KERN_ALERT "Unable to assign timer, exiting\n");
	  my_cleanup_module();
      return -ENOMEM;
	}
	setup_timer(ledTimer, timer_handler, 0);
	mod_timer(ledTimer, jiffies + msecs_to_jiffies(TIMER_PERIOD));

	/* Initialize timers for detecting simultaneous button presses */
	sonarTimer = (struct timer_list*) kmalloc(sizeof(struct timer_list), GFP_KERNEL);
	if (!sonarTimer) {
	  printk(KERN_ALERT "Unable to assign timer, exiting\n");
	  my_cleanup_module();
      return -ENOMEM;
	}
	/* sonarTimer->data = (unsigned long) kmalloc(sizeof(int), GFP_KERNEL); */
	sonarTimer->data = 0;
	haltSonar = 0;
	sonarCount = 0;
	prevSonarRead = 0;
	slowCount = 0;
	setup_timer(sonarTimer, sonarTimerHandler, 0);
	mod_timer(sonarTimer, jiffies + usecs_to_jiffies(SONAR_PERIOD));

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
 * Modifies PWM registers, 
 * regNum refers to which PWM registers are being modified, either 0 or 1
 * 10 LSB for dutyCycle and periodValue will be taken
 * 6 LSB for preScale will be taken
 * See Xscale documentation for others
 **********************************************************************************/
static int modPWMRegs(int regNum, int preScale, int dutyCycle, int periodValue) {

  /* Save appropriate registers */
  if (regNum == 0) {
    PWM_CTRL0 |= 0x0000003F & preScale;
    PWM_PWDUTY0 |= 0x000003FF & dutyCycle;
    PWM_PERVAL0 |= 0x000003FF & periodValue;
  }
  else if (regNum == 1) {
    PWM_CTRL1 |= 0x0000003F & preScale;
    PWM_PWDUTY1 |= 0x000003FF & dutyCycle;
    PWM_PERVAL1 |= 0x000003FF & periodValue;

  }

  return 1; /* Change this to something meaningfull */
}

/***********************************************************************************
 * Timer for detecting simulataneous button presses
 **********************************************************************************/
static void sonarTimerHandler(unsigned long data) {
  if (pxa_gpio_get_value(SONAR)) 
    sonarCount++;
  mod_timer(sonarTimer, jiffies + usecs_to_jiffies(SONAR_PERIOD));
}


/***********************************************************************************
 * Timer Callback for count
 **********************************************************************************/
static void timer_handler(unsigned long data) {
  /* int l0, l1, l2, l3; */
  slowCount++;
  /* if (sonarCount > 100) { */
    printk(KERN_ALERT "sonarCount:%d\n", sonarCount);
    printk(KERN_ALERT "slow count:%d\n", slowCount);
    slowCount = 0;
    sonarCount = 0;
  /* } */


  /* Reset timer */
  mod_timer(ledTimer, jiffies + msecs_to_jiffies(TIMER_PERIOD));
}

static void my_cleanup_module(void)
{
  unregister_chrdev(GPIO_MAJOR, "mygpio");
  free_irq(IRQ_GPIO(SONAR), NULL);
  free_irq(IRQ_GPIO(BTN1), NULL);

  if(ledTimer) kfree(ledTimer);
  if(bufKern)  kfree(bufKern);
}



/***********************************************************************************
 * Takes user input and modifies period 
 **********************************************************************************/
static ssize_t gpio_write( struct file *filp, const char __user *buff, unsigned long len, void *data) {
  char op;
  char val;
  memset(bufKern, 0, 256);

  if (copy_from_user(bufKern, buff, len)) { return -EFAULT;}
  sscanf(buff,"%c%c", &op, &val);
  
  /* Change timer frequency */
  if(strlen(buff) == 3)
  {
      if (op == 'f') {
          if(val >= '1' && val <= '9')
          {
              timer_period = 1000 * (val - '0') / 4;
          }
          else {
              printk(KERN_INFO "Invalid Input\n");
          }
      }
      /* Change timer value */
      else if (op == 'v') {
	if(val >= '0' && val <= '9')
          {
	    count = val - '0';
	    pxa_set_cken(CKEN0_PWM0, count&0x1);
	    pxa_gpio_set_value(LED1, count&0x2);
	    pxa_gpio_set_value(LED2, count&0x4);
	    pxa_gpio_set_value(LED3, count&0x8);
          }
	else if(val >= 'a' && val <= 'f')
          {
	    count = val - 'a' + 10;
	    pxa_set_cken(CKEN0_PWM0, count&0x1);
	    pxa_gpio_set_value(LED1, count&0x2);
	    pxa_gpio_set_value(LED2, count&0x4);
	    pxa_gpio_set_value(LED3, count&0x8);
          }
	else {
	  printk(KERN_INFO "Invalid Input\n");
	}
      }
      else if (op == 'b') {
	if(val >= '1' && val <= '3') {
	  switch (val) {
	  case '1': 
	    modPWMRegs(0, 63, 10, 10); /* Clock every 5us , 50% duty cycle,  */
	    bLevel = 1;
	    break;
	  case '2': 
	    modPWMRegs(0, 63, 10, 30); /* Clock every 5us , 33% duty cycle,  */
	    bLevel = 2;
	    break;
	  case '3':
	    modPWMRegs(0, 63, 10, 100); /* Clock every 5us , 10% duty cycle,  */
	    bLevel = 3;
	    break;
	  }
	}
	else {
	  printk(KERN_INFO "Invalid Input\n");
	}
      }

      else { 
        printk(KERN_INFO "Invalid Input\n");
      }
  }
  else { 
        printk(KERN_INFO "Invalid Input\n");
  }
  return len;
}


/***********************************************************************************
 * Prints relavent program data
 **********************************************************************************/
static ssize_t gpio_read(struct file* filp, char* buf, size_t _count, loff_t* f_pos) {
  int len = 0;
  memset(bufKern, 0, 256);

  int unit = timer_period/1000;
  int deci = timer_period - (unit * 1000);
  
  if (*f_pos > 0) 
    return 0;

  /* Print Data To Buffer */
  len += sprintf((bufKern+len), "CounterValue: %d\n", count);
  len += sprintf((bufKern+len), "CounterPeriod: %d.%d secs\n", unit, deci);
  switch (bLevel) {
  case 1: 
    len += sprintf((bufKern+len), "Brightness Level: 50%% duty cycle\n");
    break;

  case 2: 
    len += sprintf((bufKern+len), "Brightness Level: 25%% duty cycle\n");
    break;

  case 3:
    len += sprintf((bufKern+len), "Brightness Level: 10%% duty cycle\n");
    break;
  }

  if (dir)
    len += sprintf((bufKern+len), "Dir: %s\n", "UP");
  else
    len += sprintf((bufKern+len), "Dir: %s\n", "DOWN");

  if (!en) 
    len += sprintf((bufKern+len), "State: %s\n", "RUNNING");
  else 
    len += sprintf((bufKern+len), "State: %s\n", "STOPPED");

  if (copy_to_user(buf, bufKern, len)) return -EFAULT;

  printk(KERN_ALERT "Test\n");
  
  *f_pos += len;
  return len;
}

module_init(my_init_module);
module_exit(my_cleanup_module);
