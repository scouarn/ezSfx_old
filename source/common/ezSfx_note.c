#include "ezSfx_note.h"
#include "ezSfx_core.h"

#include <stdlib.h>
#include <string.h>


typedef struct node_t {

	EZ_Note_t *note;
	double endTime;

	struct node_t *next;

} node_t;


static node_t  list_root = {0};
static node_t* list_head = &list_root;


/* add a note to the queue */
void EZ_sfx_play(EZ_Note_t* note) {

	node_t* newNode = malloc( sizeof(node_t) );
	double time = EZ_sfx_getTime();

	newNode->note = note;
	newNode->endTime = time + note->length;
	

	/* insert newNode at the begining */
	newNode->next = list_head;
	list_head = newNode;

}


EZ_Sample_t EZ_note_output(double time, int channel) {

	double sf = 0.0;
	int nb_notes = 0;

	node_t *node = list_head;


	while (node->next != NULL) {

		sf += node->note->amp * node->note->oscillator(time, node->note->pitch, node->note->osciParam);
		nb_notes ++;


		/* Remove node from list by copying the next on the current and freeing the next */
		if (time >= node->endTime) {

			node_t* next = node->next;

			node->next = next->next;
			node->note = next->note;

			free(next);

		}
		else {
			node = node->next;
		}


	}

	/* nb_notes division : prevent saturation */
	return (EZ_Sample_t)(sf / nb_notes * SAMPLE_MAX);
}
