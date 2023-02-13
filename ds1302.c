#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>

#include <linux/delay.h>

#include <linux/gpio/consumer.h>
#include <linux/pinctrl/consumer.h>

#include <linux/rtc.h>
#include <linux/bcd.h>

#define HIGH	(1)
#define LOW 	(0)

enum {
	ds1302_RTC_SECONDS_REG_ADDR = 0x00,
 	ds1302_RTC_MINUTES_REG_ADDR,			
 	ds1302_RTC_HOURS_REG_ADDR,			
 	ds1302_RTC_DAY_REG_ADDR,				
 	ds1302_RTC_DATE_REG_ADDR,				
 	ds1302_RTC_MONTH_CENTURY_REG_ADDR,		
 	ds1302_RTC_YEAR_REG_ADDR,
	ds1302_RTC_MAX_TIME_REG_NUM,
};

#define ds1302_RTC_HOURS_12_AM_PM		(0x40)
#define ds1302_RTC_HOURS_24				(0x00)

#define ds1302_RTC_CONTROL_REG_ADDR		(0x07)

#define DS1302_RTC_NAME "ds1302"

static int debug;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "Turn on/off debug level log(default 0)");

#define pr_info(fmt, arg...) do {                   \
    if (debug)                          \
        printk(KERN_DEBUG pr_fmt("%s: " fmt),  __func__, ##arg);         \
} while (0)

struct ds1302_pins {
	struct gpio_desc *ce;
	struct gpio_desc *sclk;
	struct gpio_desc *io;
};

struct ds1302_dev {
	struct rtc_device *rtc;
	struct ds1302_pins pins;
	struct device *dev;
	int epoch;
};

static int ds1302_rtc_send_command(struct ds1302_dev *ds1302, int ck, int reg, int flag)
{
	gpiod_set_value(ds1302->pins.ce, HIGH);


	


	gpiod_set_value(ds1302->pins.ce, LOW);
	
	return 0;
}

static int ds1302_rtc_write(struct ds1302_dev *ds1302)
{
	//ds1302_rtc_send_command();
	gpiod_set_value(ds1302->pins.ce, HIGH);

	


	gpiod_set_value(ds1302->pins.ce, LOW);

	return 0;
}

static int ds1302_rtc_read(struct ds1302_dev *ds1302)
{
	gpiod_set_value(ds1302->pins.ce, HIGH);

	


	gpiod_set_value(ds1302->pins.ce, LOW);
	
	return 0;
}

static int ds1302_rtc_get_time(struct device *dev, struct rtc_time *t)
{
	struct ds1302_dev *ds1302 = dev_get_drvdata(dev);	
	int ret = 0, regs[ds1302_RTC_MAX_TIME_REG_NUM];

	pr_info("[%s][%s]", __func__, __LINE__);

	if(ret)
		return ret;

	t->tm_sec = bcd2bin(regs[ds1302_RTC_SECONDS_REG_ADDR]);
	t->tm_min = bcd2bin(regs[ds1302_RTC_MINUTES_REG_ADDR]);
	t->tm_hour = bcd2bin(regs[ds1302_RTC_HOURS_REG_ADDR]); // 
	t->tm_wday = bcd2bin(regs[ds1302_RTC_DAY_REG_ADDR]);

	t->tm_mday = bcd2bin(regs[ds1302_RTC_DATE_REG_ADDR]);
	t->tm_mon = bcd2bin(regs[ds1302_RTC_MONTH_CENTURY_REG_ADDR]) - 1;
	t->tm_year = bcd2bin(regs[ds1302_RTC_YEAR_REG_ADDR]) + ds1302->epoch;

	pr_info("sec:%d, min:%d, hour:%d, wday:%d, mday%d, mon:%d, year:%d\n", t->tm_sec, t->tm_min, t->tm_hour, t->tm_wday, t->tm_mday, t->tm_mon, t->tm_year);
		
	return 0;
}

static int ds1302_rtc_set_time(struct device *dev, struct rtc_time *t)
{
	pr_info("[%s][%s]", __func__, __LINE__);

	return 0;
}

static const struct rtc_class_ops ds1302_rtc_ops = {
	.read_time = ds1302_rtc_get_time,
	.set_time = ds1302_rtc_set_time,
};

static int ds1302_rtc_dt_pinctrl(struct ds1302_dev *ds1302)
{
	struct pinctrl *p;
	struct pinctrl_state *s;
	int ret = 0;

	p = pinctrl_get(ds1302->dev);
	if(IS_ERR(p)) {
		dev_err(ds1302->dev, "failed to get pinctrl\n");
		return p;
	}

	s = pinctrl_lookup_state(p, "default");
	if(IS_ERR(s)) {
		dev_err(ds1302->dev, "failed to look up default pinctrl state\n");
		pinctrl_put(p);
		return ERR_PTR(ret);
	}

	ret = pinctrl_select_state(p ,s);
	if(ret < 0) {
		dev_err(ds1302->dev, "failed to set pinctrl state\n");
		pinctrl_put(p);
		return ERR_PTR(ret);
	}

	pinctrl_put(p);
	pr_info("Configured the pins mux & conf of ds1302\n");

	return 0;
}

static int ds1302_rtc_dt_gpio(struct ds1302_dev *ds1302)
{
	struct gpio_desc *ce, *sclk, *io;
	int ret = 0;

	ce = gpiod_get_index(ds1302->dev, DS1302_RTC_NAME, 0, GPIOD_OUT_LOW);
	if(IS_ERR(ce)) {
		dev_err(ds1302->dev, "ce pin devicetree error\n");
		return ERR_PTR(ce);
	}
	ds1302->pins.ce = ce;	

	ce = gpiod_get_index(ds1302->dev, DS1302_RTC_NAME, 1, GPIOD_OUT_LOW);
	if(IS_ERR(sclk)) {
		dev_err(ds1302->dev, "sclk pin devicetree error\n");
		return ERR_PTR(sclk);
	}
	ds1302->pins.sclk = sclk;	

	ce = gpiod_get_index(ds1302->dev, DS1302_RTC_NAME, 2, GPIOD_OUT_LOW);
	if(IS_ERR(io)) {
		dev_err(ds1302->dev, "io pin devicetree error\n");
		return ERR_PTR(io);
	}
	ds1302->pins.io = io;

	pr_info("Configured the gpios of ds1302\n");

	return 0;
}

static int ds1302_rtc_dt(struct ds1302_dev *ds1302)
{
	int ret = 0;
	
	ret = ds1302_rtc_dt_pinctrl(ds1302);
	if(ret)
		return ret;

	ret = ds1302_rtc_dt_gpio(ds1302);
	if(ret)
		return ret;

	pr_info("Parsed devicetree of ds1302\n");
	return 0;
}

static int ds1302_rtc_config(struct ds1302_dev *ds1302)
{
	ds1302_rtc_write(ds1302);
	return 0;
}

static int ds1302_rtc_probe(struct platform_device *pdev)
{
    int err = 0;
	struct ds1302_dev *ds1302;
	u32 ret = 0;

	printk("1===[%s][L:%d]", __func__, __LINE__);
	ds1302 = devm_kzalloc(&pdev->dev, sizeof(*ds1302), GFP_KERNEL);
	if(IS_ERR(ds1302))
		return PTR_ERR(ds1302);

	printk("2===[%s][L:%d]", __func__, __LINE__);
	ds1302->dev = &pdev->dev;	
	ds1302->epoch = 100;
	dev_set_drvdata(&pdev->dev, ds1302);

	printk("3===[%s][L:%d]", __func__, __LINE__);
	/*
	err = ds1302_rtc_dt(ds1302);
	if(err) {
	
	printk("4===[%s][L:%d]", __func__, __LINE__);
		return err;	
	}
	*/
	printk("5===[%s][L:%d]", __func__, __LINE__);
/*
	ds1302->rtc = devm_rtc_device_register(&pdev->dev, pdev->name, &ds1302_rtc_ops, THIS_MODULE);
	if(IS_ERR(ds1302->rtc))
		return PTR_ERR(ds1302->rtc);
*/
	pr_info("ds1302 Probe Done!!!\n");   	
	return 0;
}

static int ds1302_rtc_remove(struct platform_device *pdev)
{ 
    struct ds1302_dev *ds1302 = dev_get_drvdata(&pdev->dev);

	gpiod_put(ds1302->pins.ce);
	gpiod_put(ds1302->pins.sclk);
	gpiod_put(ds1302->pins.io);
		
	pr_info("ds1302 Removed!!!\n");

    return 0;
}

static const struct of_device_id ds1302_rtc_dts[] = {
        { .compatible = "yohda,ds1302" },
        {}
};
MODULE_DEVICE_TABLE(of, ds1302_rtc_dts);

static struct platform_driver ds1302_rtc_driver = {
        .driver = {
            .name   = DS1302_RTC_NAME,
            .owner  = THIS_MODULE,
			.of_match_table = ds1302_rtc_dts,
        },
        .probe  = ds1302_rtc_probe,
        .remove = ds1302_rtc_remove,
};
/*
static int __init ds1302_rtc_init(void)
{
		
	pr_info("ds1302 Removed!!!\n");
    return platform_driver_register(&ds1302_rtc_driver);
}

static void __exit ds1302_rtc_exit(void)
{

	pr_info("ds1302 Removed!!!\n");
    platform_driver_unregister(&ds1302_rtc_driver);
}

module_init(ds1302_rtc_init);
module_exit(ds1302_rtc_exit);
*/
module_platform_driver(ds1302_rtc_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yohan Yoon <dbsdy1235@gmail.com>");
MODULE_DESCRIPTION("DS1302 Device Driver");
