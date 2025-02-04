#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

typedef struct
{
    char *data;  // Pointer to the data
    size_t size; // Size of the data
} MemoryChunk;

// Callback function to handle response data
size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t total_size = size * nmemb;
    MemoryChunk *mem = (MemoryChunk *)userdata;

    // Allocate or expand memory for the new data
    char *new_data = realloc(mem->data, mem->size + total_size + 1);
    if (new_data == NULL)
    {
        fprintf(stderr, "Failed to allocate memory\n");
        return 0;
    }

    mem->data = new_data;
    memcpy(&(mem->data[mem->size]), ptr, total_size);
    mem->size += total_size;
    mem->data[mem->size] = '\0'; // Null-terminate the string

    return total_size;
}

int sumOfChars(char *c)
{
    int acc = 0;
    while (*c++)
        acc += *c;
    return acc;
}

int main(void)
{
    CURL *curl;
    CURLcode res;
    MemoryChunk response = {0};
    curl = curl_easy_init();
    if (!curl)
        return 1;

    curl_easy_setopt(curl, CURLOPT_URL, "https://mensa.aaronschlitt.de/meals/Griebnitzsee?date=2025-02-04&lang=de");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        return 1;

    curl_easy_cleanup(curl);

    int printed[20] = {0};

    char *buf = response.data;

    int counter = 0;
    while (*buf++)
    {
        char potentialName[100];
        char potentialPrice[100];

        if (*buf == 'n' && *(buf + 1) == 'a' && *(buf + 2) == 'm' && *(buf + 3) == 'e' && *(buf + 4) == '"' && *(buf + 5) == ':' && *(buf + 6) == '"')
        {
            buf += 7;

            int len = 0;
            while (*buf != '"')
            {
                potentialName[len] = *buf++;
                len++;
            }
            potentialName[len] = '\0';
            len = 0;
            buf += 3;
            if (*buf == 's' && *(buf + 1) == 't' && *(buf + 2) == 'u')
                buf += 14;

            while (*buf == '.' || *buf <= '9' && *buf >= '0')
            {
                potentialPrice[len] = *buf++;
                len++;
            }

            if (potentialPrice[0])
            {
                int exists = 0;
                int sum = sumOfChars(potentialName);
                for (int i = 0; i < 20 && !exists; i++)
                    exists = printed[i] == sum;

                if (!exists)
                {
                    printf("(%d) %s€ - %s\n", counter + 1, potentialPrice, potentialName);
                    printed[counter++] = sum;
                }
            }
            len = 0;
            potentialName[0] = '\0';
            potentialPrice[0] = '\0';
        }
    }

    return 0;
}