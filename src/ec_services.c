/*
    ettercap -- services list module

    Copyright (C) ALoR & NaGA

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    $Header: /home/drizzt/dev/sources/ettercap.cvs/ettercap_ng/src/ec_services.c,v 1.1 2003/05/31 13:30:09 alor Exp $

*/

#include <ec.h>
#include <ec_file.h>
#include <ec_proto.h>

/* globals */

static SLIST_HEAD(, entry) serv_head;

struct entry {
   u_int32 serv;
   u_int16 proto;
   char *name;
   SLIST_ENTRY(entry) next;
};

/* protos */

static void discard_servdb(void);
int services_init(void);
char * service_search(u_int32 serv, u_int8 proto);

/*****************************************/


static void discard_servdb(void)
{
   struct entry *l;

   while (SLIST_FIRST(&serv_head) != NULL) {
      l = SLIST_FIRST(&serv_head);
      SLIST_REMOVE_HEAD(&serv_head, next);
      free(l->name);
      free(l);
   }

   DEBUG_MSG("ATEXIT: discard_servdb");
   
   return;
}


int services_init(void)
{
   struct entry *s;
   FILE *f;

   char line[128];
   char name[32];
   char type[8];
   u_int32 serv;
   u_int8 proto;
   int i = 0;

   /* errors are handled by the function */
   f = open_data(SERVICES_NAMES, "r");

   while (fgets(line, 80, f) != 0) {

      if (sscanf(line, "%32s%u/%8s", name, &serv, type) != 3)
         continue;

      /* only tcp and udp services */
      if (!strcmp(type, "tcp"))
         proto = NL_TYPE_TCP;
      else if (!strcmp(type, "udp"))
         proto = NL_TYPE_UDP;
      else
         continue;
      
      /* skip comments */
      if (strstr(name, "#"))
         continue;

      s = calloc(1, sizeof(struct entry));
      ON_ERROR(s, NULL, "Can't allocate memory");

      s->name = strdup(name);
      s->serv = htons(serv);
      s->proto = proto;
      
      SLIST_INSERT_HEAD(&serv_head, s, next);

      i++;

   }

   DEBUG_MSG("serv_init -- %d services loaded", i);
   USER_MSG("%4d services loaded\n", i);
   
   fclose(f);

   atexit(discard_servdb);

   return i;
}

/*
 * search for a service and 
 * return its name
 */

char * service_search(u_int32 serv, u_int8 proto)
{
   struct entry *s;

   SLIST_FOREACH(s, &serv_head, next) {
      if (s->serv == serv && s->proto == proto)
         return (s->name);
   }

   /* empty name for unknown services */
   return "";
}


/* EOF */

// vim:ts=3:expandtab

