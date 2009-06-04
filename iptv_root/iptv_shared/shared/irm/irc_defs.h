#ifndef __IRC_DEF__
#define __IRC_DEF__



#define NICKLEN         30

/* Necessary to put 9 here instead of 10  if
 * s_msg.c/m_nick has been corrected.  This
 * preserves compatibility with old * servers --msa
 */

#define MAX_DATE_STRING 32      /* maximum string length for a date string */

#define USERLEN             10
#define REALLEN             50
#define TOPICLEN            307
#define KILLLEN             400
#define CHANNELLEN          30  // Valor original = 32
#define PASSWDLEN           63
#define KEYLEN              23
#define BUFSIZE             512 /* WARNING: *DONT* CHANGE THIS!!!! */
#define MAXRECIPIENTS       20
#define MAXBANS             100
#define KICKLEN             300    
#define NMAXFLAGS_CMD       6
#define FLAGLEN             2
#define FLAGSIGNALLEN       1
#define UMODELEN            20
#define MAXJOINS            10

#define MOTDLINELEN         90

#define MAXSILES            10
#define MAXSILELENGTH       128

#define MAXDCCALLOW         5
#define DCC_LINK_ME         0x01    /* This is my dcc allow */


#define HOSTLEN             63      /* Length of hostname.  Updated to */
#define VERSIONLEN          63      /* Length of version name.   */
#define DESCLEN             63      /* Length of description (Ex. server description).    */

/* comply with RFC1123 */

#define HOSTIPLEN           15      /* Length of dotted quad form of IP */
#define NICKLEN             30
#define USERIDLEN           NICKLEN+USERLEN+HOSTLEN+2  // (nick!user@host)
#define EMAILLEN            USERLEN+HOSTLEN+1          // (user@host)


#define CMDLEN              30      /* Tamanho maximo de um comando */
#define PROTOCOLLEN          3
#define MEDIANAMELEN        30      /* Nome da Media */
#define SERVICELEN          30
#define MEDIACLASSLEN       10


/* User Modes */
#define UMODE_o     0x00001	/* umode +o - Global Oper */
#define UMODE_O     0x00002	/* umode +O - Local Oper */
#define UMODE_i     0x00004	/* umode +i - Invisible */
#define UMODE_w     0x00008	/* umode +w - Get wallops */
#define UMODE_s     0x00010	/* umode +s - Server notices */
#define UMODE_c     0x00020	/* umode +c - Client connections/exits */
#define UMODE_r     0x00040	/* umode +r - registered nick */
#define UMODE_k     0x00080	/* umode +k - Server kill messages */
#define UMODE_f     0x00100	/* umode +f - Server flood messages */

//#define UMODE_y     0x00200	/* umode +y - Stats/links */
#define UMODE_C     0x00200	/* umode +C - user has captured CAM */

#define UMODE_d     0x00400	/* umode +d - Debug info */

#define UMODE_T     0x00800	/* umode +T - No voice detected (mic in mute) */

#define UMODE_g     0x01000	/* umode +g - Globops */
#define UMODE_b     0x02000	/* umode +b - Chatops */
#define UMODE_a     0x04000	/* umode +a - Services Admin */
#define UMODE_A     0x08000     /* umode +A - Server Admin */
//#define UMODE_n     0x10000	/* umode +n - Routing Notices */
#define UMODE_W     0x10000	/* umode +W - User is away */
#define UMODE_h     0x20000     /* umode +h - Helper */
#define UMODE_m     0x40000     /* umode +m - media notices */
#define UMODE_R     0x80000     /* unmode +R - No non registered msgs */
//#define UMODE_D    0x100000    /* umode +D - pseudo/hidden, has seen dcc
#define UMODE_x    0x100000    /* umode +D - pseudo/hidden, has seen dcc/msgs
				 * warning message */
#define UMODE_e    0x200000    /* umode +e - error notices */
#define UMODE_M    0x400000    /* umode +M - can change channel modes */
#define UMODE_U    0x800000    /* umode +U - can change user modes */

/* Channel Related macros follow */

/* Channel related flags */

#define	CHFL_CHANOP     0x0001	/* Channel operator */
#define	CHFL_VOICE      0x0002	/* the power to speak */
#define	CHFL_DEOPPED 	0x0004	/* deopped by us, modes need to be bounced */
#define	CHFL_BAN	0x0008	/* ban channel flag */

/* ban mask types */

#define MTYP_FULL      0x01    /* mask is nick!user@host */
#define MTYP_USERHOST  0x02    /* mask is user@host */
#define MTYP_HOST      0x04    /* mask is host only */

/* Channel Visibility macros */

#define	MODE_CHANOP	CHFL_CHANOP
#define	MODE_VOICE	CHFL_VOICE
//#define	MODE_DEOPPED    CHFL_DEOPPED
#define MODE_ONEAUDIO    0x000004
#define	MODE_PRIVATE     0x000008
#define	MODE_SECRET      0x000010
#define	MODE_MODERATED   0x000020
#define	MODE_TOPICLIMIT  0x000040
#define	MODE_INVITEONLY  0x000080
#define	MODE_NOPRIVMSGS  0x000100
#define	MODE_KEY         0x000200
#define	MODE_BAN         0x000400
#define	MODE_LIMIT       0x000800
#define MODE_REGISTERED  0x001000
#define MODE_REGONLY     0x002000
#define MODE_BITRATE     0x004000
#define MODE_NAMESLIMIT  0x008000
#define MODE_MULTIVOICE  0x010000
#define MODE_BANDSHARE   0x020000
#define MODE_VOICELIMIT  0x040000
#define MODE_AUTODEVOICE 0x080000
#define MODE_AUDIOMUTE   0x100000
#define MODE_CHGUBITRATE 0x200000
#define MODE_PERSISTENT  0x400000

//#define MODE_NOCOLOR    0x4000
//#define MODE_OPERONLY   0x8000

//#define MODE_DEFAULT     MODE_NOPRIVMSGS | MODE_TOPICLIMIT | MODE_MULTIVOICE | MODE_BANDSHARE | MODE_VOICELIMIT | MODE_ONEAUDIO | MODE_BITRATE
#define MODE_DEFAULT     MODE_NOPRIVMSGS | MODE_TOPICLIMIT | MODE_MULTIVOICE | MODE_BANDSHARE | MODE_VOICELIMIT | MODE_BITRATE

#define MODE_BANDSHAREDEF      7
#define MODE_VOICELIMITDEF     7
#define MODE_ONEAUDIODEF    1500
#define MODE_BITRATEDEF       33


#endif  // __IRC_DEF__


