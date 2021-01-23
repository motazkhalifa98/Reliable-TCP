#include "../include/simulator.h"
#include <iostream>
#include <queue>
/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/
//Author: Motaz Khalifa
/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
struct pkt sentPacket;

int receivedState = 0;

bool senderFree = true;

struct pkt receivedPacket;

std::queue<msg> buffer;


/* called from layer 5, passed the data to be sent to other side */

void sendMessage()
{
  std::cout << "SEND MESSAGE() \n";
  if (senderFree){
  std::cout << "Sender is free.. \n";
  senderFree = false;

  struct msg firstmessage = buffer.front();
  std::cout << "Start Parsing \n";
  int checksum = 0;
  for (int i = 0; i < 20; i++){
    std::cout << firstmessage.data[i];
    sentPacket.payload[i] = firstmessage.data[i];  //copying data from message to packet
    checksum = checksum + int(firstmessage.data[i]);    //start calculating checksum
  }
  //perform checksum
  std::cout << "\n";
  std::cout << "seq: ";
  std::cout << sentPacket.seqnum << "\n";
  checksum = checksum + sentPacket.seqnum;
  std::cout << "ack: ";
  std::cout << sentPacket.acknum << "\n";
  checksum = checksum + sentPacket.acknum;
  std::cout << "Checksum + seq + ack: ";
  std::cout << checksum << "\n";
  sentPacket.checksum = checksum;
  tolayer3(0, sentPacket);
  std::cout << "Sending packet to layer 3B.. \n";
  starttimer(0, 15);
  std::cout << "Starting timer.. \n";
  buffer.pop();
  std::cout << "Popped queue.. \n";
} else {
  std::cout << "Sender is busy.. \n";
}
}
void A_output(struct msg message)
{ 
  std::cout << "A OUTPUT: (1) \n";
  std::cout << "Pushing message to queue.. \n";
  buffer.push(message);
  std::cout << "Calling sendMessage().. \n";
  sendMessage();

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
  std::cout << "A INPUT: (3) \n";
  bool checked = true;
  int checksum = 0;
  for (int i = 0; i < 20; i++){
      std::cout << packet.payload[i];
      checksum = checksum + int(packet.payload[i]);   //adding up asci values
    }
  checksum = checksum + packet.acknum;
  checksum = checksum + packet.seqnum;
  std::cout << "\n";
  std::cout << "packet.acknum: ";
  std::cout << packet.acknum << "\n";
  std::cout << "packet.seqnum: ";
  std::cout << packet.seqnum << "\n";
  std::cout << "packet.checksum: ";
  std::cout << packet.checksum << "\n";
  std::cout << "Calculated checksum: ";
  std::cout << checksum << "\n"; 
  if (checksum == packet.checksum){
    checked = true;
  } else {
    checked = false;
  }


  if ((!(checked)) || (packet.acknum != sentPacket.seqnum)){
    std::cout << "Checksum not equal or ack not equal to seq \n";
    //do nothing
  } else if ((checked) && (packet.acknum == sentPacket.seqnum)){
    std::cout << "Checksum equal and ack equal to seq \n";
    std::cout << "Stop timer, free sender \n";
    stoptimer(0);
    senderFree = true;
    // packet sent
    //flip seq
    if (sentPacket.seqnum == 0){
      sentPacket.seqnum = 1;
    } else if (sentPacket.seqnum == 1){
      sentPacket.seqnum = 0;
    }
    //cleaning buffer
    /*  while (!buffer.empty()){
      
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "Cleaning buffer.. \n";
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";
      std::cout << "\n";
      sendMessage();
    }
    */
  }
  std::cout << "\n";
  std::cout << "\n";
  std::cout << "\n";
  std::cout << "\n";
  std::cout << "\n";
  std::cout << "\n";
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
  std::cout << "\n";
  std::cout << "TIMER INTERRUPT!! \n";
  std::cout << "Resend packet to layer 3B, restart timer \n";
  std::cout << "\n";
  tolayer3(0, sentPacket);
  starttimer(0, 15);
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  sentPacket.seqnum = 0;
  sentPacket.acknum = 0;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
  std::cout << "B INPUT: (2) \n";
  std::cout << "State: ";
  std::cout << receivedState << "\n";
  bool checked = true;
  int checksum = 0;
  for (int i = 0; i < 20; i++){
      checksum = checksum + int(packet.payload[i]);   //adding up asci values
      std::cout << packet.payload[i];
    }
  checksum = checksum + packet.acknum;
  checksum = checksum + packet.seqnum;
  std::cout << "\n";
  std::cout << "packet.acknum: ";
  std::cout << packet.acknum << "\n";
  std::cout << "packet.seqnum: ";
  std::cout << packet.seqnum << "\n";
  std::cout << "packet.checksum: ";
  std::cout << packet.checksum << "\n";
  std::cout << "Calculated checksum: ";
  std::cout << checksum << "\n"; 
  if (checksum == packet.checksum){
    checked = true;
  } else {
    checked = false;
  }





  if ((checked) && (packet.seqnum == receivedState)){
    std::cout << "Checksum and seqnum match \n";
    char data[20];

    for (int i = 0; i < 20; i++){
      data[i] = packet.payload[i];   //copying data from packet to data
      receivedPacket.payload[i] = packet.payload[i];  //copying data from send to receive for checksum only
    }
    std::cout << "Sending to layer 5B \n";    
    tolayer5(1, data);
    
    //updated ack and checksum
    receivedPacket.acknum = packet.seqnum;
    std::cout << "New acknum: ";
    std::cout << receivedPacket.acknum;
    std::cout << "\n";
    receivedPacket.checksum = checksum;
    std::cout << "New checksum: ";
    std::cout << receivedPacket.checksum;
    std::cout << "\n";
    std::cout << "Sending to layer 3A \n";   
    tolayer3(1, receivedPacket);

    //flip state
    if (receivedState == 0){
      receivedState = 1;
    } else if (receivedState == 1){
      receivedState = 0;
    }

  } else if ((!(checked)) || (packet.seqnum != receivedState)){
    std::cout << "Checksum and seqnum do not match \n";
    std::cout << "send packet to layer 3A \n";
    tolayer3(1, receivedPacket);
  }

}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
  receivedPacket.seqnum = 0;
  receivedPacket.acknum = 0;
}
