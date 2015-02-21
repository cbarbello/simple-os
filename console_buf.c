#include "scheduler.h"
//#include "kos.h"
#include "jval.h"
#include "dllist.h"
#include "console_buf.h"

#include <stdlib.h>

void console_buf_read(){
	char c;
	while(1){
		P_kt_sem(consoleWait);
		c = console_read();

		if(console_read_buf_length < 256){
			console_read_buf_length += 1;
			Jval jval = new_jval_i(c);
			dll_append(console_read_buf, jval);
			V_kt_sem(nelem);
		}
	}
	kt_exit();
}