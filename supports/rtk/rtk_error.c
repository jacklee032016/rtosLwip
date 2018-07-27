#include <rtk_types.h>
#include <rtk_error.h>

const char * rtk_errMsg_get(int32 errCode)
{
    switch(errCode)
    {
    /* 0x0000xxxx for common error code */
    case RT_ERR_OK:                     return "OK";
    case RT_ERR_INPUT:                  return "invalid input parameter";
    case RT_ERR_UNIT_ID:                return "invalid unit id";
    case RT_ERR_PORT_ID:                return "invalid port id";
    case RT_ERR_PORT_MASK:              return "invalid port mask";
    case RT_ERR_PORT_LINKDOWN:          return "link down port status";
    case RT_ERR_ENTRY_INDEX:            return "invalid entry index";
    case RT_ERR_NULL_POINTER:           return "input parameter is null pointer";
    case RT_ERR_QUEUE_ID:               return "invalid queue id";
    case RT_ERR_QUEUE_NUM:              return "invalid queue number";
    case RT_ERR_BUSYWAIT_TIMEOUT:       return "busy watting time out";
    case RT_ERR_MAC:                    return "invalid mac address";
    case RT_ERR_OUT_OF_RANGE:           return "input parameter out of range";
    case RT_ERR_CHIP_NOT_SUPPORTED:     return "functions not supported by this chip model";
    case RT_ERR_SMI:                    return "SMI error";
    case RT_ERR_NOT_INIT:               return "The module is not initial";
    case RT_ERR_CHIP_NOT_FOUND:         return "The chip can not found";
    case RT_ERR_NOT_ALLOWED:            return "actions not allowed by the function";
    case RT_ERR_DRIVER_NOT_FOUND:       return "The driver can not found ";
    case RT_ERR_SEM_LOCK_FAILED:        return "Failed to lock semaphore";
    case RT_ERR_SEM_UNLOCK_FAILED:      return "Failed to unlock semaphore";
    case RT_ERR_ENABLE:                 return "invalid enable parameter";
    case RT_ERR_TBL_FULL:               return "input table full";
    case RT_ERR_NO_ACK:                 return "I2C slave does not output ACK signal";
    
    /* 0x0001xxxx for vlan */
    case RT_ERR_VLAN_VID:               return "invalid vid";
    case RT_ERR_VLAN_PRIORITY:          return "invalid 1p priority";
    case RT_ERR_VLAN_EMPTY_ENTRY:       return "emtpy entry of vlan table";
    case RT_ERR_VLAN_ACCEPT_FRAME_TYPE: return "invalid accept frame type";
    case RT_ERR_VLAN_EXIST:             return "vlan is exist";
    case RT_ERR_VLAN_ENTRY_NOT_FOUND:   return "specified vlan entry not found ";
    case RT_ERR_VLAN_PORT_MBR_EXIST:    return "member port exist in the specified vlan";
    case RT_ERR_VLAN_PROTO_AND_PORT:    return "invalid protocol and port based vlan";

    /* 0x0002xxxx for svlan */
    case RT_ERR_SVLAN_ENTRY_INDEX:      return "invalid svid entry no ";
    case RT_ERR_SVLAN_ETHER_TYPE:       return "invalid SVLAN ether type";
    case RT_ERR_SVLAN_TABLE_FULL:       return "no empty entry in SVLAN table";
    case RT_ERR_SVLAN_ENTRY_NOT_FOUND:  return "specified svlan entry not found ";
    case RT_ERR_SVLAN_EXIST:            return "SVLAN entry is exist";
    case RT_ERR_SVLAN_VID:              return "invalid svid ";
    /* 0x0003xxxx for MSTP */
    case RT_ERR_MSTI:                   return "invalid msti";
    case RT_ERR_MSTP_STATE:             return "invalid spanning tree status";
    case RT_ERR_MSTI_EXIST:             return "MSTI exist";
    case RT_ERR_MSTI_NOT_EXIST:         return "MSTI not exist ";
    case RT_ERR_STP_BRIDGE_PRIORITY:    return "invalid bridge priority ";
    case RT_ERR_STP_FORWARDDELAY:       return "invalid forward delay";
    case RT_ERR_STP_MAXAGE:             return "invalid max age";
    case RT_ERR_STP_HELLOTIME:          return "invalid hello time";
    case RT_ERR_STP_PORT_PRIORITY:      return "invalid port priority";
    case RT_ERR_STP_PORT_PATH_COST:     return "invalid port path cost";
    case RT_ERR_STP_PARAMETER_CHANGE_INT_CLEAR:      return "invalid port path cost";
    case RT_ERR_STP_PARAMETER_CHANGE_INT_DONE:       return "invalid port path cost";
    
    /* 0x0004xxxx for BUCKET */
    case RT_ERR_TIMESLOT:               return "invalid time slot";
    case RT_ERR_TOKEN:                  return "invalid token amount";
    case RT_ERR_RATE:                   return "invalid rate";
    case RT_ERR_TICK:                   return "invalid tick";
        
    /* 0x0005xxxx for RMA */
    case RT_ERR_RMA_ADDR:               return "invalid rma mac address";
    case RT_ERR_RMA_ACTION:             return "invalid rma action";
        
    /* 0x0006xxxx for L2 */
    case RT_ERR_L2_HASH_KEY:            return "invalid L2 Hash key";
    case RT_ERR_L2_HASH_INDEX:          return "invalid L2 Hash index";
    case RT_ERR_L2_CAM_INDEX:           return "invalid L2 CAM index";
    case RT_ERR_L2_ENRTYSEL:            return "invalid EntrySel";
    case RT_ERR_L2_INDEXTABLE_INDEX:    return "invalid L2 index table(=portMask table) index";
    case RT_ERR_LIMITED_L2ENTRY_NUM:    return "invalid limited L2 entry number";
    case RT_ERR_L2_AGGREG_PORT:         return "this aggregated port is not the lowest physical";
    case RT_ERR_L2_FID:                 return "invalid fid";
    case RT_ERR_L2_RVID:                return "invalid cvid ";
    case RT_ERR_L2_NO_EMPTY_ENTRY:      return "no empty entry in L2 table";
    case RT_ERR_L2_ENTRY_NOTFOUND:      return "specified entry not found";
    case RT_ERR_L2_INDEXTBL_FULL:       return "the L2 index table is full";
    case RT_ERR_L2_INVALID_FLOWTYPE:    return "invalid L2 flow type";
    case RT_ERR_L2_L2UNI_PARAM:         return "invalid L2 unicast parameter";
    case RT_ERR_L2_L2MULTI_PARAM:       return "invalid L2 multicast parameter ";
    case RT_ERR_L2_IPMULTI_PARAM:       return "invalid L2 ip multicast parameter ";
    case RT_ERR_L2_PARTIAL_HASH_KEY:    return "invalid L2 partial Hash key ";
    case RT_ERR_L2_EMPTY_ENTRY:         return "the entry is empty(invalid)";
    case RT_ERR_L2_FLUSH_TYPE:          return "the flush type is invalid";
    case RT_ERR_L2_NO_CPU_PORT:         return "CPU port not exist";

    /* 0x0007xxxx for FILTER (PIE) */
    case RT_ERR_FILTER_BLOCKNUM:        return "invalid block number ";
    case RT_ERR_FILTER_ENTRYIDX:        return "invalid entry index";
    case RT_ERR_FILTER_CUTLINE:         return "invalid cutline value";
    case RT_ERR_FILTER_FLOWTBLBLOCK:    return "block belongs to flow table ";
    case RT_ERR_FILTER_INACLBLOCK:      return "block belongs to ingress ACL";
    case RT_ERR_FILTER_ACTION:          return "action doesn't consist to entry type";
    case RT_ERR_FILTER_INACL_RULENUM:   return "invalid ACL rulenum";
    case RT_ERR_FILTER_INACL_TYPE:      return "entry type isn't an ingress ACL rule";
    case RT_ERR_FILTER_INACL_EXIST:     return "ACL entry is already exit ";
    case RT_ERR_FILTER_INACL_EMPTY:     return "ACL entry is empty ";
    case RT_ERR_FILTER_FLOWTBL_TYPE:    return "entry type isn't an flow table rule";
    case RT_ERR_FILTER_FLOWTBL_RULENUM: return "invalid flow table rulenum ";
    case RT_ERR_FILTER_FLOWTBL_EMPTY:   return "flow table entry is empty";
    case RT_ERR_FILTER_FLOWTBL_EXIST:   return "flow table entry is already exist ";
    case RT_ERR_FILTER_METER_ID:        return "invalid metering id ";
    case RT_ERR_FILTER_LOG_ID:          return "invalid log id ";
    case RT_ERR_FILTER_INACL_NONE_BEGIN_IDX:    return "entry index is not starting index of a group of rules";
    case RT_ERR_FILTER_ACTION_NUMBER:           return "too many ACL action types ";
    case RT_ERR_FILTER_ACTION_NOTFOUND:         return "ACL action not found ";
    case RT_ERR_FILTER_RULEFIELD_NO:            return "ACL rule field number error";
    case RT_ERR_FILTER_RULEFIELD_NOTFOUND:      return "ACL rule field number not found in rule";
    case RT_ERR_FILTER_RULEAPPLY_FAILED:        return "Apply ACL rule to ASIC failed ";
    case RT_ERR_FILTER_ACTIONAPPLY_FAILED:      return "Apply ACL action to ASIC failed";
    case RT_ERR_FILTER_REVERSE:                 return "Apply ACL hit reverse to ASIC failed";
    case RT_ERR_FILTER_POLICER_NOTFOUND:        return "policer entry not found";
    case RT_ERR_FILTER_COUNTER_NOTFOUND:        return "counter entry not found";
    
    /* 0x0008xxxx for ACL Rate Limit */
    case RT_ERR_ACLRL_HTHR:             return "invalid high threshold";
    case RT_ERR_ACLRL_TIMESLOT:         return "invalid time slot";
    case RT_ERR_ACLRL_TOKEN:            return "invalid token amount";
    case RT_ERR_ACLRL_RATE:             return "invalid rate ";
    /* 0x0009xxxx for Link aggregation */
    case RT_ERR_LA_CPUPORT:             return "CPU port can not be aggregated port";
    case RT_ERR_LA_TRUNK_ID:            return "invalid trunk id ";
    case RT_ERR_LA_PORTMASK:            return "invalid port mask ";
    case RT_ERR_LA_HASHMASK:            return "invalid hash mask ";
    case RT_ERR_LA_DUMB:                return "this API should be used in 802.1ad dumb mode";
    case RT_ERR_LA_PORTNUM_DUMB:        return "it can only aggregate at most four ports when 802.1ad dumb mode";
    case RT_ERR_LA_PORTNUM_NORMAL:      return "it can only aggregate at most eight ports when 802.1ad normal mode";
    case RT_ERR_LA_MEMBER_OVERLAP:      return "the specified port mask is overlapped with other group";
    case RT_ERR_LA_NOT_MEMBER_PORT:     return "the port is not a member port of the trunk";
    case RT_ERR_LA_TRUNK_NOT_EXIST:     return "the trunk doesn't exist ";

    /* 0x000axxxx for storm filter */
    case RT_ERR_SFC_TICK_PERIOD:        return "invalid SFC tick period ";
    case RT_ERR_SFC_UNKNOWN_GROUP:      return "Unknown Storm filter group ";
    /* 0x000bxxxx for pattern match */
    case RT_ERR_PM_MASK:                return "invalid pattern length. Pattern length should be 8 ";
    case RT_ERR_PM_LENGTH:              return "invalid pattern match mask: first byte must care";
    case RT_ERR_PM_MODE:                return "invalid pattern match mode ";
    /* 0x000cxxxx for input bandwidth control */
    case RT_ERR_INBW_TICK_PERIOD:       return "invalid tick period for input bandwidth control ";
    case RT_ERR_INBW_TOKEN_AMOUNT:      return "invalid amount of token for input bandwidth control ";
    case RT_ERR_INBW_FCON_VALUE:        return "invalid flow control ON threshold value for input bandwidth control";
    case RT_ERR_INBW_FCOFF_VALUE:       return "invalid flow control OFF threshold value for input bandwidth control";
    case RT_ERR_INBW_FC_ALLOWANCE:      return "invalid allowance of incomming packet for input bandwidth control";
    case RT_ERR_INBW_RATE:              return "invalid input bandwidth ";
    /* 0x000dxxxx for QoS */
    case RT_ERR_QOS_1P_PRIORITY:        return "invalid 802.1P priority ";
    case RT_ERR_QOS_DSCP_VALUE:         return "invalid DSCP value ";
    case RT_ERR_QOS_INT_PRIORITY:       return "invalid internal priority";
    case RT_ERR_QOS_SEL_DSCP_PRI:       return "invalid DSCP selection priority";
    case RT_ERR_QOS_SEL_PORT_PRI:       return "invalid port selection priority ";
    case RT_ERR_QOS_SEL_IN_ACL_PRI:     return "invalid ingress ACL selection priority ";
    case RT_ERR_QOS_SEL_CLASS_PRI:      return "invalid classifier selection priority ";
    case RT_ERR_QOS_EBW_RATE:           return "invalid egress bandwidth rate ";
    case RT_ERR_QOS_SCHE_TYPE:          return "invalid QoS scheduling type";
    case RT_ERR_QOS_QUEUE_WEIGHT:       return "invalid Queue weight";
    case RT_ERR_QOS_SEL_PRI_SOURCE:     return "invalid selection of priority source";
    case RT_ERR_QOS_SEL_PRI_GROUP:      return "invalid selection of table index";
    case RT_ERR_QOS_1P_DEI:             return "invalid dei";
    case RT_ERR_QOS_SCHEDULE:           return "illegal schedule ";
    case RT_ERR_QOS_INGRESS_LEAKY:      return "invalid ingress leaky buket id ";
    case RT_ERR_QOS_RATE:               return "invalid rate";
    /* 0x000exxxx for port ability */
    case RT_ERR_PHY_PAGE_ID:            return "invalid PHY page id ";
    case RT_ERR_PHY_REG_ID:             return "invalid PHY reg id";
    case RT_ERR_PHY_DATAMASK:           return "invalid PHY data mask ";
    case RT_ERR_PHY_AUTO_NEGO_MODE:     return "invalid PHY auto-negotiation mode";
    case RT_ERR_PHY_SPEED:              return "invalid PHY speed setting";
    case RT_ERR_PHY_DUPLEX:             return "invalid PHY duplex setting";
    case RT_ERR_PHY_FORCE_ABILITY:      return "invalid PHY force mode ability parameter ";
    case RT_ERR_PHY_FORCE_1000:         return "invalid PHY force mode 1G speed setting";
    case RT_ERR_PHY_TXRX:               return "invalid PHY tx/rx ";
    case RT_ERR_MAC_FORCE_1000:         return "invalid force mac to 1000Mbps";

    
    /* 0x000fxxxx for mirror */
    case RT_ERR_MIRROR_DIRECTION:       return "invalid error mirror direction";
    case RT_ERR_MIRROR_SESSION_FULL:    return "mirroring session is full ";
    case RT_ERR_MIRROR_SESSION_NOEXIST: return "mirroring session not exist";
    case RT_ERR_MIRROR_PORT_EXIST:      return "mirroring port already exists";
    case RT_ERR_MIRROR_PORT_NOT_EXIST:  return "mirroring port does not exists";
    case RT_ERR_MIRROR_PORT_FULL:       return "Exceeds maximum number of supported mirroring port ";

    /* 0x0010xxxx for stat */
    case RT_ERR_STAT_INVALID_GLOBAL_CNTR:   return "Invalid Global Counter ";
    case RT_ERR_STAT_INVALID_PORT_CNTR: return "Invalid Port Counter ";
    case RT_ERR_STAT_GLOBAL_CNTR_FAIL:  return "Could not retrieve/reset Global Counter";
    case RT_ERR_STAT_PORT_CNTR_FAIL:    return "Could not retrieve/reset Port Counter ";

    /* 0x0011xxxx for dot1x */
    case RT_ERR_DOT1X_INVALID_DIRECTION:return "Invalid Authentication Direction";
    case RT_ERR_DOT1X_PORTBASEDPNEN:    return "Port-based enable port error ";
    case RT_ERR_DOT1X_PORTBASEDAUTH:    return "Port-based auth port error ";
    case RT_ERR_DOT1X_PORTBASEDOPDIR:   return "Port-based opdir error ";
    case RT_ERR_DOT1X_MACBASEDPNEN:     return "MAC-based enable port error ";
    case RT_ERR_DOT1X_MACBASEDOPDIR:    return "MAC-based opdir error ";
    case RT_ERR_DOT1X_PROC:             return "unauthorized behavior error ";
    case RT_ERR_DOT1X_GVLANIDX:         return "guest vlan index error";
    case RT_ERR_DOT1X_GVLANTALK:        return "guest vlan OPDIR error ";
    case RT_ERR_DOT1X_MAC_PORT_MISMATCH:return "Auth MAC and port mismatch eror ";

    /* 0x0012xxxx for WOL */
    case RT_ERR_WOL_INVALID_NODEID:     return "Invalid node id ";
    case RT_ERR_WOL_CPU_PORT_NOT_EN:    return "port is not configured as cpu port";

    default:		                    return "Unknown error";
    }
}
        
