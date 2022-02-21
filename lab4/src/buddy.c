#include "buddy.h"
#include "uart.h"
#include "math.h"

BUDDY *buddy_list[FRAME_MAX_NUMBER];
BUDDY  buddy_block[255];
char   buddy_index[255];

int find_available_index(){
    for(int i = 1; i < 255; i++){
        if(buddy_index[i] == '0')
            return i;
    }
    return -1;
}

void buddy_init(){
    for(int i = 0; i < FRAME_MAX_NUMBER; i++)
        buddy_list[i] = nullptr;

    buddy_list[FRAME_MAX_NUMBER - 1] = &buddy_block[0];
    buddy_block[0].used = 0;
    buddy_block[0].id   = 0;
    buddy_block[0].number_of_frame = pow(2, (FRAME_MAX_NUMBER - 1));
    buddy_block[0].address = MEM_LOW;

    for(int i = 0; i < 255; i++)
        buddy_index[i] = '0';
    buddy_index[0] = '1';
}

void push_into_mem_list(int index, BUDDY *current_node){
    if(buddy_list[index] == nullptr){
        buddy_list[index] = current_node;
        return;
    }
    BUDDY *tmp = buddy_list[index];
    while(tmp->sibling != nullptr)
        tmp = tmp->sibling;
    tmp->sibling = current_node;
}

void pop_from_mem_list(int index, BUDDY *current_node){
    current_node->used = 0;
    buddy_index[current_node->id] = '0';
    BUDDY *tmp = buddy_list[index];
    if(buddy_list[index] == current_node){
        if(buddy_list[index]->sibling == nullptr)
            buddy_list[index] = nullptr;
        else
            buddy_list[index] = tmp->sibling;
        return;
    }
    while(1){
        if(tmp->sibling == current_node){
            tmp->sibling = current_node->sibling;
            break;
        }
        else
            tmp = tmp->sibling;
    }
}

void *memory_alloc(int size){

    int size_in_frame = size >> 12;
    if(size % FRAME_SIZE != 0)
        size_in_frame++;
    int power = log2(size_in_frame);
    BUDDY *tmp = nullptr;
    int index;
    for (index = power; index < FRAME_MAX_NUMBER; index++ ){
        if ( buddy_list[index] == nullptr )
            continue;

        tmp = buddy_list[index];
        while ( tmp != nullptr ) {
            if ( tmp->used )
                tmp = tmp->sibling;
            else
                break;
            
        }

        if ( tmp != nullptr )
            break;
    }

    if ( tmp == nullptr ){
        printf_s( "No available memory for allocation\n" );
        return nullptr;
    }

    int buddy_self_index;

    while ((tmp->number_of_frame) / 2 >= size_in_frame && index > 0){
        tmp->used = 1;

        buddy_self_index = find_available_index();
        tmp->left        = &buddy_block[buddy_self_index];
        tmp->left->id = buddy_self_index;
        buddy_index[buddy_self_index] = '1';

        buddy_self_index   = find_available_index();
        tmp->right        = &buddy_block[buddy_self_index];
        tmp->right->id = buddy_self_index;
        buddy_index[buddy_self_index] = '1';

        tmp->left->parent           = tmp;
        tmp->left->number_of_frame  = (tmp->number_of_frame) / 2;
        tmp->left->address          = tmp->address;
        tmp->left->used             = 0;
        tmp->left->sibling          = nullptr;

        tmp->right->parent           = tmp;
        tmp->right->number_of_frame  = ( tmp->number_of_frame ) / 2;
        tmp->right->address          = ( tmp->address ) + ( ( tmp->number_of_frame / 2 ) * FRAME_SIZE );
        tmp->right->used             = 0;
        tmp->right->sibling          = nullptr;

        index--;
        push_into_mem_list (index, tmp->left);
        push_into_mem_list (index, tmp->right);

        printf_s("Memory ");
        printf_i(tmp->id);
        printf_s(" (size :");
        printf_i(tmp->number_of_frame * FRAME_SIZE);
        printf_s(") ");
        printf_s(" (address :");
        printf_h(tmp->address);
        printf_s(") ");
        printf_s("splite into ");
        printf_i(tmp->left->id);
        printf_s(" (size :");
        printf_i(tmp->right->number_of_frame * FRAME_SIZE);
        printf_s(") ");
        printf_s(" (address :");
        printf_h(tmp->left->address);
        printf_s(") ");
        printf_c(' ');
        printf_i(tmp->right->id);
        printf_s(" (size :");
        printf_i(tmp->right->number_of_frame * FRAME_SIZE);
        printf_s(") ");
        printf_s(" (address :");
        printf_h(tmp->right->address);
        printf_s(") ");
        printf_c('\n');

        tmp = tmp->left;
    }
    tmp->used = 1;
    printf_s("allocate size :");
    printf_i(tmp->number_of_frame * FRAME_SIZE);
    printf_c(' ');
    printf_s("allocate id :");
    printf_i(tmp->id);
    printf_c(' ');
    printf_s("allocate start address :");
    printf_h(tmp->address);
    printf_c(' ');
    printf_c('\n');
    return (void *)(tmp->address);
}


void memory_free(void *addr){
    int f = 0;
    int index;
    BUDDY *tmp = nullptr;
    for(index = 0; index < FRAME_MAX_NUMBER && f == 0; index++){
        tmp = buddy_list[index];
        while(tmp != 0){
            if(tmp->address == (long)addr){
                f = 1;
                tmp->used = 0;
                break;
            }
            else{
                tmp = tmp->sibling;
            }
        }
    }

    do{

        tmp = tmp->parent;
        if(!tmp->left->used && !tmp->right->used){
            tmp->used = 0;

            printf_s("memory ");
            printf_i(tmp->left->id);
            printf_c(' ');
            printf_i(tmp->right->id);
            printf_c(' ');
            printf_i(tmp->left->number_of_frame * FRAME_SIZE);
            printf_s(" merge into ");
            printf_i(tmp->id);
            printf_c(' ');
            printf_i(tmp->number_of_frame * FRAME_SIZE);
            printf_c('\n');

            pop_from_mem_list(index - 1, tmp->left);
            pop_from_mem_list(index - 1, tmp->right);


        }
        else    break;

        index ++;
    }while(tmp != &buddy_index[0]);
}