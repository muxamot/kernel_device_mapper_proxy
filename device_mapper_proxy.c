/*

    Device Mapper Proxy (dmp)
    
    Device mapper target that
    creates a proxy device and counts some statistics
    

*/

#include <linux/device-mapper.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/bio.h>


/*

    Structure for dmp-specific data

*/

struct dmp_data 
{
    struct dm_dev* dev;
    
};


/*

    Device mapper constuctor function
    Called when block device of dmp type is created  
    by 'dmsetup create' command

*/

static int device_mapper_proxy_ctr(struct dm_target* ti, unsigned int argc, char** argv)
{
    struct dmp_data* dmpd;
    int ret;

    //check arg count
    //there are only 1 argument - device name
    if (argc != 1) 
    {
        printk(KERN_CRIT "\n Invalid number of arguments.\n");
        ti->error = "Invalid argument count";
        return -EINVAL;
    }

    //allocate memory for dmp_data structure
    dmpd = kmalloc(sizeof(struct dmp_data), GFP_KERNEL);

    //check if allocation was successful
    if(dmpd == NULL)
    {
        printk(KERN_CRIT "\n\nDMPD pointer is NULL!\n\n");
        ti->error = "dmp: allocation failed (out of memory?)";
        return -ENOMEM;
    }
    
    //get underlying device from table and set its structure
    ret = dm_get_device(ti, argv[0], dm_table_get_mode(ti->table), &dmpd->dev);
    if (ret) 
    {
        ti->error = "dmp: Device lookup failed";
        goto bad;
    }

    //store dmp structure as target-specific data in dm_target
    ti->private = dmpd;

    return 0;

    bad:
    printk(KERN_CRIT "\n\n Device mapper proxy constuctor exited with error! \n\n");
    kfree(dmpd);
    return ret;
}


/*

    Maps bio to underlying device
    Called when device is removed

*/

static int device_mapper_proxy_map(struct dm_target* ti, struct bio* bio)
{
    //checking io type
    switch (bio_op(bio)) 
    {
    case REQ_OP_READ:
        //read
        
        break;
    case REQ_OP_WRITE:
        //write

        break;
    default:
        return DM_MAPIO_KILL;
    }

    //submit bio
    struct dmp_data* dmpd = (struct dmp_data*) ti->private;
    bio_set_dev(bio, dmpd->dev->bdev);
    submit_bio(bio);
    
    return DM_MAPIO_SUBMITTED;
}


/*

    Device mapper destructor function 

*/

static void device_mapper_proxy_dtr(struct dm_target* ti)
{
    struct dmp_data* dmpd = (struct dmp_data*) ti->private;       
    dm_put_device(ti, dmpd->dev);
    kfree(dmpd);
    printk(KERN_CRIT "\n Device mapper destructor function \n");               
}


/*

    target_type structure
    describes a target parameters, sets contructor, mapper and destructor functions

*/

static struct target_type device_mapper_proxy = 
{    
    .name = "dmp",
    .version = {0,0,1},
    .module = THIS_MODULE,
    .ctr = device_mapper_proxy_ctr,
    .dtr = device_mapper_proxy_dtr,
    .map = device_mapper_proxy_map,
};


/*

    Kernel moudle functions:

*/


/*

    Register module or show message with error code

*/

static int device_mapper_proxy_init(void)
{
    int r = dm_register_target(&device_mapper_proxy);

    if (r < 0)
    {
        printk("register failed %d", r);
    }

    return r;
}


/*

    Unregister module

*/

static void device_mapper_proxy_exit(void)
{
    dm_unregister_target(&device_mapper_proxy);
}

module_init(device_mapper_proxy_init);
module_exit(device_mapper_proxy_exit);

MODULE_AUTHOR("Mikhail Motylenok <q3.inform@ya.ru>");
MODULE_DESCRIPTION(DM_NAME " proxy device counting some io statistics");
MODULE_LICENSE("GPL");