#ifndef MURADIN_BASE_ERROR_TYPE_H__
#define  MURADIN_BASE_ERROR_TYPE_H__

namespace muradin{

	typedef	struct err_msg_info_{
		int			code;
		char		msg[256];
	}err_msg;
}
#endif
