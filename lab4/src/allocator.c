#include "allocator.h"

long new_addr = (long)((volatile unsigned int *)(0x400000));

allocated_frame_t allocate_list[2];
allocated_frame_t free_allocate_frame[20];
CHUNK free_chunk[100];
char frame[20],chunk[100];

void init_allocator(){
    buddy_init();
    for(int i = 0; i < 2; i++){
        allocate_list[i].next = nullptr;
        allocate_list[i].first = nullptr;
        allocate_list[i].isfull = 2;
        allocate_list[i].allocate_size = 0;
    }
    for(int i = 0; i < 20; i++)
        frame[i] = '0';
    for(int i = 0; i < 100; i ++)
        chunk[i] = '0';
}
int find_first_frame(){
    for(int i = 0; i < 20; i++)
        if(frame[i] =='0'){
            frame[i] = '1';
            return i;
        }
    return -1;
}
int find_first_chunk(){
    for(int i = 0; i < 100; i++)
        if(chunk[i] == '0'){
            chunk[i] = '1';
            return i;
        }
    return -1;
}

void* fix_malloc(int size){

    allocated_frame_t *tmp = &allocate_list[0];
    int alloc_size;

    if(size <= 16)
        alloc_size = 16;
    else if(size <= 32)
        alloc_size = 32;
    else if(size <= 64)
        alloc_size = 64;
    else if(size <= 128)
        alloc_size = 128;
    else if(size <= 256)
        alloc_size = 256;
    else if(size <= 512)
        alloc_size = 512;
    else if(size <= 1024)
        alloc_size = 1024;
    else if(size <= 2048)
        alloc_size = 2048;



    if(tmp->next == nullptr && tmp->isfull == 2){
        long *frame_addr = (long*)memory_alloc(FRAME_SIZE);
        int i = find_first_frame();
        int j = find_first_chunk();
        free_allocate_frame[i].allocate_size = alloc_size;
        free_allocate_frame[i].isfull = 0;
        free_allocate_frame[i].start_address = frame_addr;
        free_allocate_frame[i].next = nullptr;


        free_chunk[j].isused = 1;
        free_chunk[j].start_address = frame_addr;
        free_chunk[j].next = nullptr;

        
        free_allocate_frame[i].first = &free_chunk[j];
        tmp->next = &free_allocate_frame[i];
        return (void *)free_chunk[j].start_address;
    }


    while((tmp->allocate_size != alloc_size && tmp->next) || tmp->isfull == 1){
        tmp = tmp->next;
    }

    if(tmp->allocate_size == alloc_size){
        if(tmp->isfull == 0){
            CHUNK *chunk_tmp = tmp->first;
            while(chunk_tmp->isused == 1 && chunk_tmp->next){
                chunk_tmp = chunk_tmp->next;
            }
            
            if(chunk_tmp->isused == 0){ // exist block
                chunk_tmp->isused = 1;
                return (void *)chunk_tmp->start_address;
            }
            if(chunk_tmp->next == nullptr){
                int j = find_first_chunk();
                free_chunk[j].isused = 1;
                free_chunk[j].start_address = (chunk_tmp->start_address + alloc_size);
                free_chunk[j].next = nullptr;
                chunk_tmp->next = &free_chunk[j];
                if(free_chunk[j].start_address + alloc_size == tmp->start_address + 4096){
                    tmp->isfull = 1;
                }
                return (void *)free_chunk[j].start_address;
            }
        }
        else{
            long *frame_addr = (long*)memory_alloc(FRAME_SIZE);
            int i = find_first_frame();
            int j = find_first_chunk();
            free_allocate_frame[i].allocate_size = alloc_size;
            free_allocate_frame[i].isfull = 0;
            free_allocate_frame[i].start_address = frame_addr;
            free_allocate_frame[i].next = nullptr;
            free_allocate_frame[i].first = &free_chunk[j];

            while(tmp->next)
                tmp = tmp->next;
            tmp->next = &free_allocate_frame[i];

            free_chunk[j].isused = 1;
            free_chunk[j].start_address = frame_addr;
            free_chunk[j].next = nullptr;

            return (void *)free_chunk[j].start_address;
        }
    }
    else{
        long *frame_addr = (long*)memory_alloc(FRAME_SIZE);
        int i = find_first_frame();
        int j = find_first_chunk();
        free_allocate_frame[i].allocate_size = alloc_size;
        free_allocate_frame[i].isfull = 0;
        free_allocate_frame[i].start_address = frame_addr;
        free_allocate_frame[i].next = nullptr;
        free_allocate_frame[i].first = &free_chunk[j];

        while(tmp->next)
            tmp = tmp->next;
        tmp->next = &free_allocate_frame[i];

        free_chunk[j].isused = 1;
        free_chunk[j].start_address = frame_addr;
        free_chunk[j].next = nullptr;

        return (void *)free_chunk[j].start_address;
    }

    printf_s("Unable to allocate space!!\n");
    return nullptr;

}



void *dynamic_malloc(int size){

    if(size <= 2048)
        return (void *)fix_malloc(size);
    else{
        int alloc_size;
        if(size <= 4096)
            alloc_size = FRAME_SIZE;
        else if(size <= 8192)
            alloc_size = FRAME_SIZE * 2;
        else if(size <= 16384)
            alloc_size = FRAME_SIZE * 3;
        else if(size <= 32768)
            alloc_size = FRAME_SIZE * 4;
        else if(size <= 65536)
            alloc_size = FRAME_SIZE * 5;
        else if(size <= 131072)
            alloc_size = FRAME_SIZE * 6;
        else if(size <= 262144)
            alloc_size = FRAME_SIZE * 7;
        else if(size <= 524288)
            alloc_size = FRAME_SIZE * 8;
        else{
            printf_s("no enough mem space\n");
            return nullptr;
        }
            
        allocated_frame_t *tmp = &allocate_list[1];

        while(tmp->next != nullptr && tmp->isfull == 1)
            tmp = tmp->next;

        long *frame_addr = (long *)memory_alloc(alloc_size);

        if(tmp->isfull == 0){
            tmp->start_address = frame_addr;
            tmp->isfull = 1;
            return frame_addr;
        }
        else{
            int i = find_first_frame();
            free_allocate_frame[i].start_address = frame_addr;
            free_allocate_frame[i].allocate_size = -1;
            free_allocate_frame[i].isfull = 1;
            free_allocate_frame[i].first = nullptr;
            free_allocate_frame[i].next = nullptr;
            tmp->next = &free_allocate_frame[i];
            return free_allocate_frame[i].start_address;
        }
    }
}


void free(int *address){
    allocated_frame_t *tmp = &allocate_list[0];

    if(tmp->next == nullptr){
        allocated_frame_t *tmp2 = &allocate_list[1];
        while(tmp2->isfull == 2 || (tmp2->start_address != address && tmp2->next))
            tmp2 = tmp2->next;
        if(tmp2->start_address != address){
            printf_s("no allocate\n");
            return ;
        }
        else{
            memory_free(address);
            tmp2->isfull = 0;
            return;
        }
    }
    /*printf_s("--------------------------\n");
    printf_i(tmp->isfull);
    printf_c('\n');
    printf_h(tmp->start_address);
    printf_c('\n');
    printf_h((int)address);
    printf_c('\n');
    printf_h((int)address - tmp->start_address);
    printf_c('\n');
    printf_s("--------------------------\n");
    printf_c('\n');*/
    while((((int)address - tmp->start_address >= 4096 || (int)address - tmp->start_address < 0) && tmp->next) || tmp->isfull == 2){
        tmp = tmp->next;
    }
    if((int)address - tmp->start_address >= 4096 || (int)address - tmp->start_address < 0){
        allocated_frame_t *tmp2 = &allocate_list[1];
        while(tmp2->isfull == 2 || (tmp2->start_address != address && tmp2->next)){
            tmp2 = tmp2->next;
        }
        if(tmp2->start_address != address){
            printf_s("no allocated\n");
            return ;
        }
        else{
            memory_free(address);
            tmp2->isfull = 0;
        }
    }
    else{
        CHUNK *chunk_tmp = tmp->first;
        while(chunk_tmp->start_address != address){
           chunk_tmp = chunk_tmp->next;
        }
        printf_s("free address :");
        printf_h((unsigned int)chunk_tmp->start_address);
        printf_c('\n');
        chunk_tmp->isused = 0;
    }
}






















void* simple_malloc(int size){
    char *addr_start = (char *)new_addr;
    addr_start += size;
    new_addr += size;
    return addr_start;
}

