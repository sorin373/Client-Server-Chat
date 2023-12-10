#ifndef __STORAGE_HPP__
#define __STORAGE_HPP__

#define LENGTH 45
#define N      26

namespace net
{
    class storage
    {
        struct node
        {
            char word[LENGTH + 1];
            struct node *next;
        };

        node *table[N];        

        public:
            storage() = default;

            bool check(const char *word);
            unsigned int hash(const char *word);
            bool load(const char *fileStructure);

            ~storage() =default;
    };
};

#endif