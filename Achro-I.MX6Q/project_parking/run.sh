insmod ../fpga_driver/lcd/fpga_text_lcd_driver.ko
mknod /dev/fpga_text_lcd c 263 0

insmod ../fpga_driver/fnd/fpga_fnd_driver.ko
mknod /dev/fpga_fnd c 261 0

insmod ../fpga_driver/hc_sr04/fpga_hc_sr04_driver.ko
mknod /dev/fpga_hc_sr04 c 244 0

insmod ../fpga_driver/push_button/fpga_push_switch_driver.ko
mknod /dev/push_switch 265 

make
./parking