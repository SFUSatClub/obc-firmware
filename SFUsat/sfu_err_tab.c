/*
 * sfu_err_tab.c
 *
 * Error table structure to be stored in Error Log Queue
 *
 * Created on:
 * 	   Author: Seleena
 * */



#include "sfu_err_tab.h"

ErrTab_t* create_err_entry(char* err_desc, ErrType_t type){

	ErrTab_t* table = malloc(sizeof(ErrTab_t));
	table->description = malloc(desc_limit * sizeof(char));

	//need to check return value == 0?
	//set timezone to NULL for now
	gettimeofday(&table->timestamp, NULL);
	strcpy(table->description, err_desc);
	table->error_type = type;

	return table;
}


void delete_err_entry(ErrTab_t* table){

	if( table != NULL ){
		if( table->description != NULL){
			free(table->description);
		}
		free(table);
	}
}
