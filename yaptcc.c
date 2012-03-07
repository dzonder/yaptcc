/******************************************************************************
 *
 *                 Yet Another Paste Tool Commandline Client
 *
 * Simple client which allows to add new paste and recieve pastes from YAPT
 * server. Uses CURL to communicate with server via HTTPS.
 * More info on project page and github.
 *
 * @author   Michal Olech <me@dzonder.net>
 * @homepage https://yapt.dzonder.net
 * @license  MIT
 *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <curl/curl.h>

#define URL    "https://yapt.dzonder.net/cc"
#define MAXLEN 255
#define CHUNK  sizeof(char)*1024U

void fatal(const char *msg)
{
   fprintf(stderr, "Fatal: %s\n", msg);
   exit(1);
}

void curl_fatal(CURLcode err)
{
   fprintf(stderr, "CURL error: %s\n", curl_easy_strerror(err));
   exit(1);
}

size_t read_paste(char **buf)
{
   size_t count = 0, size = 0;

   while (1) {
      *buf = (char*)realloc(*buf, CHUNK*(++count));
      size += fread(*buf+size, 1, CHUNK, stdin);

      if (feof(stdin))
         return size;

      if (ferror(stdin))
         fatal("fread error");
   }
}

int main(int argc, char *argv[])
{
   CURL *curl;
   CURLcode err;

   struct curl_httppost *formpost = NULL;
   struct curl_httppost *lastptr = NULL;

   char *bufcode = NULL;
   char passwd[MAXLEN+1];
   char url[MAXLEN+1];

   unsigned int pasteid;

   if (curl_global_init(CURL_GLOBAL_ALL))
      fatal("CURL global init failed");

   if (!(curl = curl_easy_init()))
      fatal("CURL easy init failed");

   if (argc == 1 || (argc == 3 && strcmp(argv[1], "-e") == 0)) {
      /* upload (possible encryption) */

      if (!read_paste(&bufcode))
         fatal("input empty");

      if (argc == 3) {
         strncpy(passwd, argv[2], MAXLEN);
         passwd[MAXLEN] = 0;
      } else {
         passwd[0] = 0;
      }

      curl_formadd(&formpost,
                   &lastptr,
                   CURLFORM_COPYNAME, "passwd",
                   CURLFORM_COPYCONTENTS, passwd,
                   CURLFORM_END);

      curl_formadd(&formpost,
                   &lastptr,
                   CURLFORM_COPYNAME, "code",
                   CURLFORM_COPYCONTENTS, bufcode,
                   CURLFORM_END);

      sprintf(url, "%s/paste", URL);
   } else if (argc == 2) {
      /* download */

      if (sscanf(argv[1], "%x", &pasteid) != 1)
         fatal("invalid id");

      sprintf(url, "%s/raw/%x", URL, pasteid);
   } else if (argc == 4 && strcmp(argv[1], "-e") == 0) {
      /* download with password */

      if (sscanf(argv[3], "%x", &pasteid) != 1)
         fatal("invalid id");

      curl_formadd(&formpost,
                   &lastptr,
                   CURLFORM_COPYNAME, "passwd",
                   CURLFORM_COPYCONTENTS, argv[2],
                   CURLFORM_END);

      sprintf(url, "%s/raw/%x", URL, pasteid);
   } else {
      printf("Usage: %s [ -e <passwd> ] [ <id> ]\n", argv[0]);
      printf("Example:\n");
      printf("  - public upload:\n");
      printf("      %s < file\n", argv[0]);
      printf("  - private upload (encryption at server-side):\n");
      printf("      %s -e password < file\n", argv[0]);
      printf("  - public download:\n");
      printf("      %s pasteid > file\n", argv[0]);
      printf("  - private download:\n");
      printf("      %s -e password pasteid > file\n", argv[0]);
      return 1;
   }

   if ((err = curl_easy_setopt(curl, CURLOPT_URL, url)) != CURLE_OK)
      curl_fatal(err);
   if ((err = curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost)) != CURLE_OK)
      curl_fatal(err);
   if ((err = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L)) != CURLE_OK)
      curl_fatal(err);

   if ((err = curl_easy_perform(curl)) != CURLE_OK)
      curl_fatal(err);

   free(bufcode);

   curl_easy_cleanup(curl);
   curl_formfree(formpost);

   return 0;
}
