#pragma once

class SeqHandler{
	uint32_t	seqNo;
public:
	SeqHandler() : seqNo(1) {}
	uint32_t get() {
		return seqNo;
	}
	bool confirmed(const uint32_t _seqNo) {
		if (seqNo == _seqNo){
			seqNo++;
			return true;
		}
		return false;
	}
	bool confirmedWithPrev(const uint32_t _seqNo) {
		if (seqNo == _seqNo){
			seqNo++;
			return true;
		}
		return  (_seqNo+1==seqNo);
	}

	void reset(){
		seqNo=1;
	}
};
