#pragma once
#include "afxwin.h"
#define FC_FAE_SOF 0x55
#define FC_FAE_EOF 0xAA
#define PC2CARD_WRITE 0x02
#define PC2CARD_READ 0x01
#define CARD2PC_DATA 0x01
#define CARD2PC_ACK 0x02

//全局寄存器
#define	device_soft_reset	0x0000
#define	device_version	0x0004
#define	Cfg_done	0x0008
#define	Can_speed	0x000C
#define	Link_status	0x0010
#define	Sw_id	0x0014
//配置寄存器
#define	Clear_cfg	0x0100
#define	Clear_done	0x0104
#define	Cfg_update	0x0108
#define	Flash_cfg_addr_wr	0x0110
#define	Flash_cfg_data_wr	0x0114
#define	Flash_cfg_addr_rd	0x0118
#define	Flash_cfg_data_rd	0x011C

//统计信息寄存器

#define	Mib_clear	0x1000
#define	net_A_recv_num	0x1010
#define	net_B_recv_num	0x1014
#define	net_C_recv_num	0x1018
#define	net_D_recv_num	0x101c
#define	net_A_send_num	0x1020
#define	net_B_send_num	0x1024
#define	net_C_send_num	0x1028
#define	net_D_send_num	0x102c
#define	can_A_recv_num	0x1030
#define	can_B_recv_num	0x1034
#define	can_C_recv_num	0x1038
#define	can_D_recv_num	0x103c
#define	can_A_send_num	0x1040
#define	can_B_send_num	0x1044
#define	can_C_send_num	0x1048
#define	can_D_send_num	0x104c

#define	FcA_recv_num	0x1050
#define	FcA_recv_byte_L	0x1054
#define	FcA_recv_byte_H	0x1058
#define	FcA_recv_crc_err	0x105c
#define	FcA_send_num	0x1060
#define	FcA_send_byte_L	0x1064
#define	FcA_send_byte_H	0x1068

#define	FcB_recv_num	0x1070
#define	FcB_recv_byte_L	0x1074
#define	FcB_recv_byte_H	0x1078
#define	FcB_recv_crc_err	0x107c
#define	FcB_send_num	0x1080
#define	FcB_send_byte_L	0x1084
#define	FcB_send_byte_H	0x1088

#define	TASK_ID_VER	0x0000
#define	TASK_ID_RESET	0x0001
#define	TASK_ID_CANSPD_WR	0x0002
#define	TASK_ID_CANSPD_RD	0x0002


#define PC_WRITE_CMD TRUE
#define PC_READ_CMD FALSE
typedef struct
{
	BOOL bReadWrite;
	UINT nRegAddr;//寄存器地址
	UINT WriteData;//对应的变量指针
	UINT* pReadData;//对应的变量指针
	//执行下一条指令的条件，
	//-1-无需应答（定时发送）0-ACK或有数返回，1-读取数据返回满足逻辑1，2-第8位是1
	int nNextCmd;

}PC_CMD_ITEM;

typedef struct
{
	UINT nRegAddr;//寄存器地址
	CString strDes;

}CMD_DESCRP;