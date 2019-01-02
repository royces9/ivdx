#ifndef KEYCONST
#define KEYCONST

enum actions {
	      _select,
	      _back,
	      _pause,
	      _scroll_down,
	      _scroll_up,
	      _scroll_diff_down,
	      _scroll_diff_up,
};

int kb_four[4];
int kb_five[5];
int kb_six[6];
int kb_seven[7];
int kb_seven_one[8];
int kb_eight[8];

int *const kb_game[] = {kb_four,
			kb_five,
			kb_six,
			kb_seven,
			kb_seven_one,
			kb_eight
};

int kb_select[2];
int kb_back[2];
int kb_pause[2];
int kb_scroll_down[2];
int kb_scroll_up[2];
int kb_scroll_diff_down[2];
int kb_scroll_diff_up[2];

int *kb_menu[7] = { kb_select,
		    kb_back,
		    kb_pause,
		    kb_scroll_down,
		    kb_scroll_up,
		    kb_scroll_diff_down,
		    kb_scroll_diff_up
};



#endif //KEYCONST
