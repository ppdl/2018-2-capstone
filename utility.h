#include <linux/skbuff.h>
#define IOTYR_IDENTIFIER	0x4ae7

struct iothdr {
	__u64	param1:8,
			param2:8,
			param3:8,
			param4:8,
			auth_key:16;
};

// Unifying function name format. attach prefix "iotyr_"
#define FDEFINE(fName, ...)		iotyr_##fName(__VA_ARGS__)

// pName: jprobe name
// hName: handler name
// fname: target function name
#define DECLARE_JPROBE(fName)	\
static struct jprobe probe_##fName = { \
  .entry = iotyr_##fName,	\
  .kp = {	\
	.symbol_name = #fName,	\
  },	\
}

#define REGISTER_JPROBE(fName)		register_jprobe(&probe_##fName)
#define UNREGISTER_JPROBE(fName)	unregister_jprobe(&probe_##fName)

