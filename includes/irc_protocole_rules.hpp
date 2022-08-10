#ifndef IRC_PROTOCOLE_RULES_HPP
	#define IRC_PROTOCOLE_RULES_HPP

// MESSAGES
#define END_MSG "\r\n"
#define MSG_MAX_SIZE 510
#define MSG_MULTI_PARAM_DELIM ","

// CHANNEL
#define CHAN_NAME_MAX_SIZE 200
#define CHAN_NAME_FORBID_SPACE " "
#define CHAN_NAME_FORBID_COMMA ","
#define CHAN_NAME_FORBID_CTRLG "\a"


#define CHAN_FLAGS_QT 7
#define CHAN_FLAGS_VALID "opsitlk"
enum chan_mask {
	CHAN_MASK_O,
	CHAN_MASK_P,
	CHAN_MASK_S,
	CHAN_MASK_I,
	CHAN_MASK_T,
	CHAN_MASK_L,
	CHAN_MASK_K
};

//            o - give/take channel operator privileges;
//            p - private channel flag;
//            s - secret channel flag;
//            i - invite-only channel flag;
//            t - topic settable by channel operator only flag;
//            l - set the user limit to channel;
//            k - set a channel key (password).


#endif