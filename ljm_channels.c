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

/** 
 * Channel module for acessing Labjack devices through LJM library.
 * Changelog: (date,who,description)
 */

// For printf
#include <stdio.h>

// For the LabJackM Library
#include <LabJackM.h>

// Channel sytem utility functions
#include "RMCIOS-functions.h"

struct ljm_device_data
{
   int channel_id;
   int handle;
   struct ljm_device_data *next_device;
} *first_device = NULL;

// Channel for handling labjack ljm devices
void ljm_device_func (struct ljm_device_data *this,
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
                     "ljm device channel - Labjack ljm library device.\r\n"
                     "create ljmdev newname\r\n"
                     "setup ljmdev | DeviceType ConnectionType Identifier\r\n"
                     "  -See labjack ljm library documentation"
                     "   for options on opening decice\r\n"
                     "write newname register value "
                     "  # Write value to register(name or id)\r\n"
                     "read newname register #read register(name or id) value\r\n"                    
                     );
      break;

   case create_rmcios:
      if (num_params < 1)
         break;
      // Allocate new data:
      this =
         (struct ljm_device_data *) malloc (sizeof (struct ljm_device_data));
      if (this == NULL)
         break;

      // Set default values:
      this->handle = 0;

      // Create the channel
      this->channel_id =
         create_channel_param (context, paramtype, param, 0,
                               (class_rmcios) ljm_device_func, this);

      //add device to list of devices:
      if (first_device == NULL)
      {
         // this is the first device in the system
         first_device = this;   
      }
      else
      {
         struct ljm_device_data *devices = first_device;
         while (devices->next_device != NULL)
            // find the last device
            devices = devices->next_device;     
         devices->next_device = this;   
         // Add this device to the list
      }
      break;

   case setup_rmcios:
      if (this == NULL)
         break;
      if (num_params < 3)       
      // Open device for first found labjack on any connection.
      {
         int err;
         err = LJM_OpenS ("LJM_dtANY", "LJM_ctANY", "LJM_idANY", &this->handle);
      }
      else      
      // Open device with user parameters
      {
         char DeviceType[256];
         char ConnectionType[256];
         char Identifier[256];
         int err;
         param_to_string (context, paramtype, param, 0,
                          sizeof (DeviceType), DeviceType);
         param_to_string (context, paramtype, param, 1,
                          sizeof (ConnectionType), ConnectionType);
         param_to_string (context, paramtype, param, 2,
                          sizeof (Identifier), Identifier);
         err =
            LJM_OpenS (DeviceType, ConnectionType, Identifier, &this->handle);
      }
      break;

      // Read and write share the same address resolving procedure:
   case write_rmcios:
   case read_rmcios:
      if (this == NULL)
         break;
      if (num_params < 1)
         break;
      {

         int address;           // Modbus address of register
         int type;              // Type of register
         int slen = param_string_alloc_size (context, paramtype, param, 0);
         // Check if register number given:
         address = param_to_integer (context, paramtype, param, 0);
         if (address == 0)      // not number -> Try as string
         {
            char sbuff[slen];
            const char *name;
            name = param_to_string (context, paramtype, param, 0, slen, sbuff);

            // Try to get address and type of named register
            LJM_NameToAddress (name, &address, &type);
         }
         else
         {
            // Get the type of register by its address 
            LJM_AddressToType (address, &type);

         }

         if (function == read_rmcios)   // read
         {
            if (num_params < 1)
               break;   // Read the register value
            if (type == LJM_STRING)     // Read string register
            {
               char str[LJM_STRING_ALLOCATION_SIZE];
               LJM_eReadAddressString (this->handle, address, str);
               return_string (context, returnv, str);
            }
            else        // Read Numeric register
            {
               double value;
               LJM_eReadAddress (this->handle, address, type, &value);
               return_float (context, returnv, (float) value);
            }
         }
         else   // write
         {
            if (num_params < 1) // No parameter to write
               break;   
            if (type == LJM_STRING)     
            // write string register
            {
               char str[LJM_STRING_ALLOCATION_SIZE];
               param_to_string (context, paramtype, param, 1,
                                sizeof (str), str);
               LJM_eWriteAddressString (this->handle, address, str);
            }
            else       
            // write numeric register
            {
               float value;
               value = param_to_float (context, paramtype, param, 1);
               LJM_eWriteAddress (this->handle, address, type, (float) value);
            }
         }
      }
      break;
   }
}

struct ljm_register_data
{
   struct ljm_device_data *device;
   int address;
   int type;

   int len_address;
   int len_type;
};

// Cannel for handling registers in a ljm device. 
void ljm_register_func (struct ljm_register_data *this,
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
                     "ljm register channel"
                     " Register for handling individual device register\r\n"
                     " create ljmreg newname\r\n"
                     " setup newname ljm_device_channel \r\n"
                     "               device_register(name or id)\r\n "
                     "               | type(AUTO) | length_register\r\n"
                     "   #type={AUTO, LJM_BYTE, LJM_STRING, LJM_UINT16"
                     "   #     , LJM_UINT32, LJM_INT32, LJM_FLOAT32, }\n"
                     " write newname value #Write to register\r\n"
                     " write newname \r\n"
                     "       #read register and send results to linked\r\n"
                     " read newname value #Read register\r\n"
                     " link newname channel\r\n");
      break;

   case create_rmcios:
      if (num_params < 1)
         break;
      // Allocate new data:
      this =
         (struct ljm_register_data *)
         malloc (sizeof (struct ljm_register_data));
      if (this == NULL)
         break;

      // Set default values:
      this->address = 0;
      this->type = 0;
      this->device = NULL;
      this->len_address = 0;
      this->len_type = 0;

      // Create the channel
      create_channel_param (context, paramtype, param, 0,
                            (class_rmcios) ljm_register_func, this);
      break;
   case setup_rmcios:
      if (this == NULL)
         break;
      if (num_params < 2)
         break;
      int device_channel = param_to_int (context, paramtype, param, 0);
      if (device_channel == 0)
         break;

      // Find the specified device:
      struct ljm_device_data *pdevice = first_device;
      while (pdevice != NULL)
      {
         if (pdevice->channel_id == device_channel) 
         // Found!
         {
            // Set the device:
            this->device = pdevice;
            break;
         }
         pdevice = pdevice->next_device;
      }
      if (this->device == NULL)
      {
         printf ("ljmreg: Could not find LJM device channel\r\n");
      }
      // Get register address for the channel
      int address; // Modbus address of register
      int type;    // Type of register
      int slen = param_string_alloc_size (context, paramtype, param, 1);
      
      // Check if register as number given:
      address = param_to_integer (context, paramtype, param, 1);
      
      if (address == 0) 
      // not number -> Try as string
      {
         char sbuff[slen];
         const char *name;
         name = param_to_string (context, paramtype, param, 1, slen, sbuff);
         
         // Get address and type of named register
         LJM_NameToAddress (name, &address, &type);
      }
      else
      {
         // Get the type of register by its address 
         LJM_AddressToType (address, &type);
      }
      this->address = address;
      this->type = type;

      if (num_params < 3)
         break;
      char buffer[20];
      const char *ptype =
         param_to_string (context, paramtype, param, 2, sizeof (buffer),
                          buffer);

      if (strcmp (ptype, "LJM_STRING") == 0)
         this->type = LJM_STRING;
      if (strcmp (ptype, "LJM_BYTE") == 0)
         this->type = LJM_BYTE;
      if (strcmp (ptype, "BYTE_ARRAY") == 0)
         this->type = LJM_STRING;
      if (strcmp (ptype, "LJM_UINT16") == 0)
         this->type = LJM_UINT16;
      if (strcmp (ptype, "LJM_UINT32") == 0)
         this->type = LJM_UINT32;
      if (strcmp (ptype, "LJM_INT32") == 0)
         this->type = LJM_INT32;
      if (strcmp (ptype, "LJM_FLOAT32") == 0)
         this->type = LJM_FLOAT32;

      if (num_params < 4)
         break; // Separate register that holds the length of data
      address = param_to_integer (context, paramtype, param, 3);
      if (address == 0) // not number -> Try as string
      {
         char sbuff[slen];
         const char *name;
         name = param_to_string (context, paramtype, param, 3, slen, sbuff);
         // Get address and type of named register
         LJM_NameToAddress (name, &address, &type);
      }
      else
      {
         // Get the type of register by its address 
         LJM_AddressToType (address, &type);
      }
      this->len_address = address;
      this->len_type = LJM_UINT32;

      break;
   case read_rmcios:
      if (this == NULL)
         break;
      if (this->device == NULL)
         break;
      if (this->type == LJM_STRING)     // Read string register
      {
         if (this->len_address == 0)
         {
            char str[LJM_STRING_ALLOCATION_SIZE];
            LJM_eReadAddressString (this->device->handle, this->address, str);
            return_string (context, returnv, str);
         }
         else   // read raw bytes
         {
            double len;
            int blen;
            LJM_eReadAddress (this->device->handle,
                              this->len_address, this->len_type, &len);
            blen = len;
            {
               char buffer[blen];
               int errorAddress;

               LJM_eReadAddressByteArray (this->device->handle, //int Handle,
                                          this->address,        //int Address,
                                          blen, // int NumBytes,
                                          buffer, // char * aBytes,
                                          &errorAddress); //int * ErrorAddress) ;

               return_buffer (context, returnv, buffer, blen);
            }
         }
      }
      else      // Read Numeric register
      {
         double value;
         LJM_eReadAddress (this->device->handle, this->address,
                           this->type, &value);
         return_float (context, returnv, (float) value);
      }
      break;
   case write_rmcios:
      if (this == NULL)
         break;
      if (this->device == NULL)
         break;
      if (num_params < 1)       // Send read to linked channels
      {
         if (this->type == LJM_STRING)  // Read string register
         {
            if (this->len_address == 0)
            {
               char str[LJM_STRING_ALLOCATION_SIZE];
               LJM_eReadAddressString (this->device->handle,
                                       this->address, str);
               write_str (context, linked_channels (context, id), str, 0);
               return_string (context, returnv, str);
            }
            else
            {
               double len;
               int blen;
               LJM_eReadAddress (this->device->handle,
                                 this->len_address, this->len_type, &len);
               blen = len;
               {
                  char buffer[blen];
                  int errorAddress;

                  LJM_eReadAddressByteArray (this->device->handle, //int Handle,
                                             this->address,     //int Address,
                                             blen,      // int NamBytes,
                                             buffer,    // char * aBytes,
                                             //int * ErrorAddress :
                                             &errorAddress);

                  write_buffer (context,
                                linked_channels (context, id), buffer, blen, 0);
                  return_buffer (context, returnv, buffer, blen);
               }
            }
         }
         else   // Read Numeric register
         {
            double value;
            LJM_eReadAddress (this->device->handle, this->address,
                              this->type, &value);
            write_f (context, linked_channels (context, id), (float) value);
            return_float (context, returnv, (float) value);
         }
      }
      else      // Write to register
      {
         if (this->type == LJM_STRING)  // write string register
         {
            char str[LJM_STRING_ALLOCATION_SIZE];
            param_to_string (context, paramtype, param, 0, sizeof (str), str);
            LJM_eWriteAddressString (this->device->handle, this->address, str);
         }
         else if (this->type == LJM_BYTE)       // Byte array
         {
            int plen;
            int psize;
            int errorAddress;

            psize = param_buffer_alloc_size (context, paramtype, param, 0);
            plen = param_buffer_length (context, paramtype, param, 0);
            {
               char buffer[psize];
               struct buffer_rmcios pb;
               // get the paremeter 
               pb = param_to_buffer (context, paramtype, param, 0,
                                     psize, buffer);

               // Write length to the length -register
               if (this->len_address != 0)
               {
                  LJM_eWriteAddress (this->device->handle,
                                     this->len_address,
                                     this->len_type, pb.length);
               }


               // Write the data
               LJM_eWriteAddressByteArray (this->device->handle, //int Handle,
                                           this->address,       //int Address,
                                           pb.length,   //int NumBytes,
                                           pb.data,     //const char * aBytes,
                                           &errorAddress); //int * ErrorAddress
            }
         }
         else   // write numeric register
         {
            float value;
            value = param_to_float (context, paramtype, param, 0);
            LJM_eWriteAddress (this->device->handle, this->address,
                               this->type, value);
         }
      }
      break;
   }
}

void __declspec (dllexport)
     __cdecl init_channels (const struct context_rmcios *context)
{
   printf ("Labjack ljm module\r\n[" VERSION_STR "]\r\n");

   create_channel_str (context, "ljmdev", (class_rmcios) ljm_device_func, NULL);
   create_channel_str (context, "ljmreg", (class_rmcios) ljm_register_func,
                       NULL);
}
