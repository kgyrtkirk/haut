#pragma once

/**
 * Selects the highest priority command which is not expired.
 *
 *	command - until structure; with priority
 *
 *	cmd0 - default command
 *	cmd1 - prio 1 command; expires after until
 *	cmd2 - prio 2 command
 *
 */
template<class VT,size_t CNT>
class DelayControlValue {
	struct DelayedValue{
		VT			value;
		uint64_t	until;
	};
	DelayedValue	dv[CNT];
public:

	DelayControlValue(){
		uint8_t	i;
		for(i=0;i<CNT;i++){
			dv[i].until=0;
		}
	}
	void command(uint8_t priority,uint64_t until,const VT&value) {
		if(priority>=CNT){
			return;
		}
		dv[priority].value=value;
		dv[priority].until=until;
	}

	const VT& getActiveValue() const{
		return dv[getActiveState()].value;
	}
	uint8_t getActiveState() const{
		uint8_t	i;
		uint64_t	now=millis();
		for(i=CNT-1;i>0;i--){
			if(dv[i].until > now){
				return i;
			}
		}
		return 0;
	}

};
