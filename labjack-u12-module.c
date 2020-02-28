/*
RMCIOS - Reactive Multipurpose Control Input Output System
Copyright (c) 2018 Frans Korhonen

RMIOS was originally developed at Institute for Atmospheric 
and Earth System Research / Physics, Faculty of Science, 
University of Helsinki, Finland

Assistance, experience and feedback from following persons have been 
critical for development of RMCIOS: Erkki Siivola, Juha Kangasluoma, 
Lauri Ahonen, Ella Häkkinen, Pasi Aalto, Joonas Enroth, Runlong Cai, 
Markku Kulmala and Tuukka Petäjä.

This file is extension to RMCIOS. This notice was encoded using utf-8.

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/. 
*/

#define DLL

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "RMCIOS-functions.h"
#include "labjack.h"

tEAnalogIn EAnalogIn;
tEAnalogOut EAnalogOut;
tEDigitalIn EDigitalIn;
tEDigitalOut EDigitalOut;

struct lja_data
{
   long idnum;
   int channel;
   int gain;
   float voltage;
};

void labjack_ai_func (struct lja_data *this,
                      const struct context_rmcios *context, int id,
                      enum function_rmcios function,
                      enum type_rmcios paramtype,
                      struct combo_rmcios *returnv,
                      int num_params, const union param_rmcios param)
{
   switch (function)
   {
   case help_rmcios:
      return_string (context, returnv,
                     "help for labjack ai. Commands:\r\n"
                     "create ljad ch_name | channel\r\n"
                     "setup ljad channel(0-11) | gain(0-7) | idnum(-1)"
                     "write ljad #aquire voltage\r\n"
                     "read ljad #read voltage\r\n");
      break;

   case create_rmcios: // params: 0=channel | 1=channel
      if (num_params < 1)
      {
         printf ("Not enough parameters\r\n");
         break;
      }

      // allocate new data
      this = (struct lja_data *) malloc (sizeof (struct lja_data)); 
      
      // create channel
      create_channel_param (context, paramtype, param, 0, 
                            (class_rmcios) labjack_ai_func, this);        

      this->idnum = -1;
      this->gain = 0;
      this->channel = 0;
      if (num_params < 2) break;
      this->channel = param_to_int (context, paramtype, param, 1);
      break;

   case setup_rmcios:  // 0=channel | 1=gain | 2=idnum
      if (this == NULL)
         break;
      if (num_params < 1)
         break;
      this->channel = param_to_int (context, paramtype, param, 0);
      if (num_params < 2)
         break;
      this->gain = param_to_int (context, paramtype, param, 1);
      if (num_params < 3)
         break;
      this->idnum = param_to_int (context, paramtype, param, 2);

   case read_rmcios:
      if (this == NULL)
         break;
      return_float (context, returnv, this->voltage);
      break;

   case write_rmcios:
      if (this == NULL)
         break;
      long overVoltage;
      EAnalogIn (&this->idnum, 0, this->channel, 
                 this->gain, &overVoltage, &this->voltage);

      write_f (context, linked_channels (context, id), this->voltage);
      break;
   }
}

void labjack_ao_func (struct lja_data *this,
                      const struct context_rmcios *context, int id,
                      enum function_rmcios function,
                      enum type_rmcios paramtype,
                      struct combo_rmcios *returnv,
                      int num_params, const union param_rmcios param)
{
   switch (function)
   {
   case help_rmcios:
      return_string (context, returnv,
                     "help for labjack ao. Commands:\r\n"
                     "create ljad ch_name | channel\r\n"
                     "setup ljad channel(0-1) | idnum(-1)"
                     "write ljad #set voltage\r\n");
      break;
   case create_rmcios:
      if (num_params < 1)
      {
         printf ("Not enough parameters\r\n");
         break;
      }

      // allocate new data
      this = (struct lja_data *) malloc (sizeof (struct lja_data));     
      
      // create channel
      create_channel_param (context, paramtype, param, 0, 
                            (class_rmcios) labjack_ao_func, this); 

      this->idnum = -1;
      this->voltage = 0;
      this->channel = 0;
      if (num_params < 2)
         break;

      this->channel = param_to_int (context, paramtype, param, 1);
      break;

   case setup_rmcios:  // 0=channel 1=idnum
      if (this == NULL)
         break;
      if (num_params < 1)
         break;
      this->channel = param_to_int (context, paramtype, param, 0);
      if (num_params < 2)
         break;
      this->idnum = param_to_int (context, paramtype, param, 1);
      break;
   case write_rmcios:
      if (this == NULL)
         break;
      this->voltage = param_to_float (context, paramtype, param, 0);
      if (this->channel == 0)
         EAnalogOut (&this->idnum, 0, this->voltage, -1.0);
      if (this->channel == 1)
         EAnalogOut (&this->idnum, 0, -1.0, this->voltage);
      write_f (context, linked_channels (context, id), this->voltage);
      break;
   case read_rmcios:
      if (this == NULL)
         break;
      return_float (context, returnv, this->voltage);
      break;
   }
}

struct ljd_data
{
   long idnum;
   int channel;
   int terminalD;
   long state;
};

void labjack_do_func (struct ljd_data *this,
                      const struct context_rmcios *context, int id,
                      enum function_rmcios function,
                      enum type_rmcios paramtype,
                      struct combo_rmcios *returnv,
                      int num_params, const union param_rmcios param)
{
   switch (function)
   {
   case help_rmcios:
      return_string (context, returnv,
                     "help for labjack digital out. Commands:\r\n"
                     "create ljdo ch_name | channel\r\n"
                     "setup ljdo channel(IO0-IO3/D0-D15) "
                     "  | Dport | idnum(-1)\r\n"
                     "write ljdo #set state\r\n"
                     "read ljdo #read latest written state\r\n");
      break;

   case create_rmcios:
      if (num_params < 1)
      {
         printf ("Not enough parameters\r\n");
         break;
      }

      // allocate new data
      this = (struct ljd_data *) malloc (sizeof (struct ljd_data)); 
      
      // create channel
      create_channel_param (context, paramtype, param, 0, 
                            (class_rmcios) labjack_do_func, this); 

      this->idnum = -1;
      this->channel = 0;
      this->terminalD = 0;
      this->state = 0;
      if (num_params < 2)
         break;
      this->channel = param_to_int (context, paramtype, param, 1);
      break;

   case setup_rmcios:  // 0=channel | 1=terminalD | 2=idnum
      if (this == NULL)
         break;
      if (num_params < 1)
         break;
      this->channel = param_to_int (context, paramtype, param, 0);
      if (num_params < 2)
         break;
      this->terminalD = param_to_int (context, paramtype, param, 1);
      if (num_params < 3)
         break;
      this->idnum = param_to_int (context, paramtype, param, 2);
      break;

   case write_rmcios:
      if (this == NULL)
         break;
      this->state = param_to_int (context, paramtype, param, 0);
      EDigitalOut (&this->idnum,
                   0, this->channel, this->terminalD, this->state);
      write_i (context, linked_channels (context, id), this->state);
      break;

   case read_rmcios:
      if (this == NULL)
         break;
      return_int (context, returnv, this->state);
      break;
   }
}

void labjack_di_func (struct ljd_data *this,
                      const struct context_rmcios *context, int id,
                      enum function_rmcios function,
                      enum type_rmcios paramtype,
                      struct combo_rmcios *returnv,
                      int num_params, const union param_rmcios param)
{
   switch (function)
   {
   case help_rmcios:
      return_string (context, returnv,
                     "help for labjack digital in. Commands:\r\n"
                     "create ljdi ch_name | channel\r\n"
                     "setup ljdi channel(IO0-IO3/D0-D15) "
                     " | Dport | idnum(-1)\r\n"
                     "write ljdi #aquire state\r\n"
                     "read ljdi #read latest aquired state\r\n");
      break;

   case create_rmcios:
      if (num_params < 1)
      {
         printf ("Not enough parameters\r\n");
         break;
      }

      // allocate new data
      this = (struct ljd_data *) malloc (sizeof (struct ljd_data));     
      
      // create channel
      create_channel_param (context, paramtype, param, 0, 
                            (class_rmcios) labjack_di_func, this); 

      this->idnum = -1;
      this->channel = 0;
      this->terminalD = 0;
      this->state = 0;
      if (num_params < 2)
         break;
      this->channel = param_to_int (context, paramtype, param, 1);
      break;

   case setup_rmcios:  // 0=channel | 1=terminalD | 2=idnum
      if (this == NULL)
         break;
      if (num_params < 1)
         break;
      this->channel = param_to_int (context, paramtype, param, 0);
      if (num_params < 2)
         break;
      this->terminalD = param_to_int (context, paramtype, param, 1);
      if (num_params < 3)
         break;
      this->idnum = param_to_int (context, paramtype, param, 2);
      break;

   case write_rmcios:
      if (this == NULL)
         break;
      EDigitalIn (&this->idnum,
                  0, this->channel, this->terminalD, &this->state);
      write_i (context, linked_channels (context, id), this->state);
      break;

   case read_rmcios:
      if (this == NULL)
         break;
      return_float (context, returnv, this->state);
      break;
   }
}

HINSTANCE hDLLInstance;

void API_ENTRY_FUNC init_channels (const struct context_rmcios *context)
{
   printf ("Labjack u12 module\r\n[" VERSION_STR "]\r\n");
   //Now try and load the DLL.
   if (hDLLInstance = LoadLibrary ("ljackuw.dll"))
   {
      //If successfully loaded, get the address of the desired functions.
      EAnalogIn = (tEAnalogIn) GetProcAddress (hDLLInstance, "EAnalogIn");
      EAnalogOut = (tEAnalogOut) GetProcAddress (hDLLInstance, "EAnalogOut");
      EDigitalOut =(tEDigitalOut) GetProcAddress (hDLLInstance, "EDigitalOut");
      EDigitalIn = (tEDigitalIn) GetProcAddress (hDLLInstance, "EDigitalIn");

      //AISample = (tAISample) GetProcAddress(hDLLInstance,"AISample");
      create_channel_str (context, "ljai", (class_rmcios)labjack_ai_func, NULL);
      create_channel_str (context, "ljao", (class_rmcios)labjack_ao_func, NULL);
      create_channel_str (context, "ljdo", (class_rmcios)labjack_do_func, NULL);
      create_channel_str (context, "ljdi", (class_rmcios)labjack_di_func, NULL);
   }
   else
   {
      printf ("Failed to load ljackuw.DLL\t\n");
   }
}
