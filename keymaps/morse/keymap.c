#include QMK_KEYBOARD_H
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <time.h>

#define HOLD_TIME 250
#define DEBOUNCE 10

typedef struct mtree {
	char letter;
	struct mtree *dash;
	struct mtree *dot;
} Morsetree;

Morsetree *morsetree(char l){
    Morsetree *new;

    if( (new = (Morsetree *)malloc(sizeof(Morsetree))) == NULL) {
        fprintf( stderr, "Memory Allocation error.\n" );
        return new;
    }
    new->letter = l;
    new->dot = NULL;
    new->dash = NULL;
    return new;
};

void add_morse(Morsetree *tree, char *morse, char letter){
    Morsetree *t;
    int i, len;

    t = tree;
    len = strlen( morse );
    for( i = 0; i < len; i++ ) {
        if( morse[i] == '.' ) {
            if( t->dot == NULL )
                t->dot = morsetree('*');
            t = t->dot;
        }
        else {
            if( t->dash == NULL )
                t->dash = morsetree('*');
            t = t->dash;
        }
    }
    t->letter = letter;
};

static Morsetree* r;
void createTree(void){
	r = morsetree('*');
	add_morse(r, ".-", 'A');
	add_morse(r, "-..." ,'B' );
	add_morse(r, "-.-.", 'C' );
	add_morse(r, "-..", 'D');
	add_morse(r, ".", 'E');
	add_morse(r, "..-.", 'F');
	add_morse(r, "--.", 'G');
	add_morse(r, "....", 'H');
	add_morse(r, "..", 'I');
	add_morse(r, ".---", 'J');
	add_morse(r, "-.-", 'K');
	add_morse(r, ".-..", 'L');
	add_morse(r, "--", 'M');
	add_morse(r, "-.", 'N');
	add_morse(r, "---", 'O');
	add_morse(r, ".--.", 'P');
	add_morse(r, "--.-", 'Q');
	add_morse(r, ".-.", 'R');
	add_morse(r, "...", 'S');
	add_morse(r, "-", 'T');
	add_morse(r, "..-", 'U');
	add_morse(r, "...-", 'V');
	add_morse(r, ".--", 'W');
	add_morse(r, "-..-", 'X');
	add_morse(r, "-.--", 'Y');
	add_morse(r, "--..", 'Z');
};

Morsetree* getNextNode(Morsetree* tree, char* code){
	Morsetree* newNode;
	if(strcmp(code,"-")){
		newNode = tree->dash;
	}
	else if(strcmp(code, ".")){
		newNode = tree->dot;
	}
	else{
		newNode = tree;
	}
	return newNode;
}

enum my_keycodes {
    MORSE = SAFE_RANGE,
	NEXT
};

static Morsetree* currentTreeNode;

void keyboard_pre_init_user(void){
	createTree();
	currentTreeNode = r;
}

static uint16_t morseHoldTimer;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	switch (keycode) {
		case MORSE:
			if (record->event.pressed) {
				morseHoldTimer = timer_read();
			}
			else{
				//release key
				if(timer_elapsed(morseHoldTimer) < HOLD_TIME){
					//send a dash
					currentTreeNode = getNextNode(currentTreeNode, "-");
				}
				else{
					//send a dot
					currentTreeNode = getNextNode(currentTreeNode, ".");
				}
			}
			return false;
			break;  
		case NEXT:
			if (record->event.pressed) {
				//does nothing
			}
			else{
				//release key
				if(currentTreeNode == r){
					//space
					register_code(KC_SPACE);
					unregister_code(KC_SPACE);
				}
				else{
					//spit out letter
					char temp = currentTreeNode->letter;
					char str[2] = "\0";
					str[0] = temp;
					send_string(str);
					//resets node back to tree head
					currentTreeNode = r;
				}
			}
			return false;
			break;
	}
	return false;
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        NEXT, /* bottom button */
        MORSE /* top button */
    )
};
