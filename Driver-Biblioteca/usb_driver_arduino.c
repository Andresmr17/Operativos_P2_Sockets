// usb_driver_arduino.c
// Driver USB personalizado para Arduino - Fase 2: Comunicación por USB Bulk

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define USB_VENDOR_ID_ARDUINO     0x1a86  
#define USB_PRODUCT_ID_ARDUINO    0x7523 

#define DEVICE_NAME "proyecto_arduino_usb"
#define CLASS_NAME "proyecto_arduino_usb"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ANDRES");
MODULE_DESCRIPTION("Driver USB para Arduino - Comunicación Bulk sin cdc_acm");

static struct usb_device *device;
static unsigned char bulk_out_endpointAddr;

static int majorNumber;
static struct class* arduinoClass = NULL;
static struct device* arduinoDevice = NULL;

static struct usb_device_id arduino_table[] = {
    { USB_DEVICE(USB_VENDOR_ID_ARDUINO, USB_PRODUCT_ID_ARDUINO) },
    {}
};
MODULE_DEVICE_TABLE(usb, arduino_table);

static int arduino_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "[proyecto_arduino_usb] Dispositivo abierto\n");
    return 0;
}

static int arduino_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "[proyecto_arduino_usb] Dispositivo cerrado\n");
    return 0;
}

static ssize_t arduino_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *ppos) {
    char buffer[64];
    int retval, actual_length;

    if (!device) return -ENODEV;

    if (count > sizeof(buffer)) count = sizeof(buffer);
    if (copy_from_user(buffer, user_buffer, count)) return -EFAULT;

    retval = usb_bulk_msg(device,
                          usb_sndbulkpipe(device, bulk_out_endpointAddr),
                          buffer,
                          count,
                          &actual_length,
                          1000);
    if (retval) {
        printk(KERN_ERR "[proyecto_arduino_usb] Error en usb_bulk_msg: %d\n", retval);
        return retval;
    }

    return actual_length;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = arduino_open,
    .release = arduino_release,
    .write = arduino_write,
};

static int arduino_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    struct usb_host_interface *iface_desc;
    struct usb_endpoint_descriptor *endpoint;
    int i;

    device = interface_to_usbdev(interface);
    iface_desc = interface->cur_altsetting;

    for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
        endpoint = &iface_desc->endpoint[i].desc;
        if (usb_endpoint_is_bulk_out(endpoint)) {
            bulk_out_endpointAddr = endpoint->bEndpointAddress;
            break;
        }
    }

    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_ERR "[proyecto_arduino_usb] No se pudo registrar chrdev\n");
        return majorNumber;
    }

    arduinoClass = class_create(CLASS_NAME);
    if (IS_ERR(arduinoClass)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ERR "[proyecto_arduino_usb] Fallo al crear clase\n");
        return PTR_ERR(arduinoClass);
    }

    arduinoDevice = device_create(arduinoClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(arduinoDevice)) {
        class_destroy(arduinoClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ERR "[proyecto_arduino_usb] Fallo al crear dispositivo\n");
        return PTR_ERR(arduinoDevice);
    }

    printk(KERN_INFO "[proyecto_arduino_usb] Arduino conectado - listo para escritura por USB\n");
    return 0;
}

static void arduino_disconnect(struct usb_interface *interface) {
    device_destroy(arduinoClass, MKDEV(majorNumber, 0));
    class_unregister(arduinoClass);
    class_destroy(arduinoClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "[proyecto_arduino_usb] Arduino desconectado\n");
}

static struct usb_driver arduino_usb_driver = {
    .name = "proyecto_arduino_usb",
    .id_table = arduino_table,
    .probe = arduino_probe,
    .disconnect = arduino_disconnect,
};

static int __init arduino_init(void) {
    printk(KERN_INFO "[proyecto_arduino_usb] Inicializando driver USB personalizado...\n");
    return usb_register(&arduino_usb_driver);
}

static void __exit arduino_exit(void) {
    usb_deregister(&arduino_usb_driver);
    printk(KERN_INFO "[proyecto_arduino_usb] Driver USB eliminado\n");
}

module_init(arduino_init);
module_exit(arduino_exit);
