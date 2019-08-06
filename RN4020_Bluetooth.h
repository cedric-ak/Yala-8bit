/* 
 * File:   RN4020_Bluetooth.h
 * Author: CEDRIC
 *
 * Created on 27 July 2019, 12:42 AM
 */

#ifndef RN4020_BLUETOOTH_H
#define	RN4020_BLUETOOTH_H

#ifdef	__cplusplus
extern "C" {
#endif

    char CMD_mode[] = "$$$\r";
    uint8_t device_name[] = "SN,Yala Device\r";
    char reset_To_Factory[] ="SF,1\r";
    char BTH_reboot[] = "r,1\r";


#ifdef	__cplusplus
}
#endif

#endif	/* RN4020_BLUETOOTH_H */

