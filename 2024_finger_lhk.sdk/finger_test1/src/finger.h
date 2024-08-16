/*
 * finger.h
 *
 *  Created on: 2024��5��17��
 *      Author: lenovo
 */

#ifndef SRC_FINGER_H_
#define SRC_FINGER_H_


extern unsigned char FPM10A_RECEICE_BUFFER[32];
extern unsigned int recv_cnt;
//extern volatile unsigned char recv_tmp[32];
extern unsigned char ucLength;
extern unsigned char finger_data_received;

void FPM10A_Cmd_Get_Img(void);
void FPM10A_Cmd_Check(void);
void Device_Check(void);
void FPM10A_Receive_Data(unsigned char ucLength);
void FPM10A_Delete_All_Fingerprint();
void FPM10A_Find_Fingerprint();
void FPM10A_Cmd_Search_Finger(void);
void FPM10A_Add_Fingerprint();
void FPM10A_Cmd_Reg_Model();
void FPM10A_Cmd_Save_Finger( unsigned int storeID );
void FINGERPRINT_Cmd_Delete_All_Finger(void);

#endif /* SRC_FINGER_H_ */
