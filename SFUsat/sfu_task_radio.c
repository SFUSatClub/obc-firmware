    strobe(SNOP);
 
    snprintf(buffer, 30, "Radio Status Registers:");
    serialSendln(buffer);
    snprintf(buffer, 30, "%02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, ",
          stat[0], stat[1], stat[2], stat[3], stat[4], stat[5], stat[6], stat[7], stat[8], stat[9],
        stat[10], stat[11], stat[12], stat[13]);
    serialSendln(buffer);
 
    if(configureRadio(SMARTRF_VALS_TX, PA_TABLE_SETTING)){
      snprintf(buffer, 30, "radio registers do not match!");
      serialSendln(buffer);
    }
 
 
    strobe(SNOP);
//attach irq
 
    strobe(SRX);
//move here ##
 
//Move ##
  return pdPASS;
}
 
BaseType_t radioCmd(char * toSend) {
  if (xQueueSendToBack(xSerialTXQueue, &toSend, 0) == pdPASS) {
    return pdPASS;
  } else {
    return pdFAIL;
  }
}