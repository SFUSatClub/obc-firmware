/*
 * sfu_err_tab.h
 *
 * Error table structure to be stored in Error Log Queue
 *
 * Created on:
 * 	   Author: Seleena
 * */

#ifndef SFUSAT_ERR_TAB_H
#define SFUSAT_ERR_TAB_H


#include <sys/time.h> //memory concerns for the embedded system?
#include <string.h>

#define desc_limit 20 //size limit for description in ErrTab_t

typedef enum { //change error types later
	ER1,
	ER2,
	ER3

} ErrType_t;

typedef struct {

	struct timeval timestamp;
	ErrType_t error_type; //Change later
	char* description;

} ErrTab_t ;


/* Initialize values for the error table structure */
ErrTab_t* create_err_entry(char* err_desc, ErrType_t type);

/* Free memory allocated for error table */
void delete_err_entry(ErrTab_t* table);



#endif /* SFUSAT_ERR_TAB_H */
