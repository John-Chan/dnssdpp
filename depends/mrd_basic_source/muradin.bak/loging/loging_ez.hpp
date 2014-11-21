
/**
 * 
 * @file       loging_ez.hpp 	
 * @author     cj@sunnysec.com.cn
 * @date       2014-1-17
 *  
 * @brief      ����־��¼��װ
 * @remark     �����ڼ�ʹ�ó���  
 * @note    
 * @e<somthing important>
 * 
 * @e Copyright (c) <year> <copyright holders>
 */


#ifndef	MURADIN_LOGING_EASY_HPP__
#define MURADIN_LOGING_EASY_HPP__

#include <muradin/loging/log/log_generater.hpp>
namespace muradin
{
namespace loging
{
	static log_sink_console global_consol_output_;
}
}
// �����̨�����־,����֤�̰߳�ȫ��Ҳ����֤�쳣��ȫ
// �÷� 
// CONSOLE_LOG(log_debug).get_line()<<"this is log msg"<<" "<<"������־��Ϣ";
#define CONSOLE_LOG(LOG_LEVEL) ( muradin::loging::ConsolLoger((LOG_CREATE_ATTRIB_OBJ((LOG_LEVEL))),global_consol_output_) )

// ���ļ������־,����֤�̰߳�ȫ��Ҳ����֤�쳣��ȫ
// �÷� 
// muradin::loging::file_writer file("log.txt","w");
// EZ_FILE_LOG(log_debug,file).get_line()<<"this is log msg"<<" "<<"������־��Ϣ";
// ��ע:�ļ��Ĵ򿪺͹ر���ʹ�÷�ά��
#define EZ_FILE_LOG(LOG_LEVEL,FILE_OBJ) ( muradin::loging::FilelLoger((LOG_CREATE_ATTRIB_OBJ((LOG_LEVEL))),FILE_OBJ) )

#endif//MURADIN_LOGING_EASY_HPP__