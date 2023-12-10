#include "storage.hpp"

#include <string>

namespace net
{
    unsigned int storage::hash(const char *word)
    {
        unsigned int hashValue = 0;

        for (unsigned int i = 0, n = strlen(word); i < n; i++)
            hashValue += toupper(word[i]);

        return hashValue % N;
    }

    bool storage::check(const char *word)
    {
        unsigned int value = hash(word);
        node *ptr = table[value];

        while (ptr != NULL)
        {
            if (strcasecmp(word, ptr->word) == NULL)
                return true;
            
            ptr = ptr->next;
        }

        return false;
    }
    
    
    bool storage::load(const char *fileStructure)
    {
        
    }
};