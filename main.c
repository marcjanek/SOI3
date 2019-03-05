#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>


#define con_count 5
#define buf_len 10

#define test 3

#if test == 1//production>>consumption

#define prod_count 20

#define con_time 10
#define prod_time 3

#elif test == 2//production<<consumption

#define prod_count 3

#define con_time 2
#define prod_time 11


#else//production==consumption

#define prod_count 4

#define con_time 5
#define prod_time 5

#endif


pthread_t consumers[con_count], producers[prod_count];

sem_t buf_sem[con_count], empty_sem[con_count], full_sem[con_count],count_sem;

int buf[con_count][buf_len], buf_counter[con_count],zmienna;

void print_queues() {
    for(int i=0,j;i<con_count;i++)
    {
        printf("items in queue %d: ",i);
        if(buf_counter[i]==0)
        {
            printf("empty");
        }
        else
        {
            for(j=0;j<buf_counter[i];j++)
            {
                if(j!=0){printf(",");}
                printf("%d", buf[i][j]);
            }
        }
        printf("\n");
    }
    printf("----------------------\n");
}

void *producer() {
    int producer_number, queue;

    for (producer_number = 0; !pthread_equal(producers[producer_number], pthread_self()) &&
                              producer_number < prod_count; producer_number++) {}
    while (1) {
        queue = rand() % con_count;
        sem_wait(&empty_sem[queue]);
        sem_wait(&buf_sem[queue]);
        sem_wait(&count_sem);
        int item = 10 + rand() % 89;
        printf("Producer %d produced %d for queue: %d\n", producer_number, item, queue);
        buf[queue][buf_counter[queue]] = item;
        ++buf_counter[queue];
        print_queues();
        sem_post(&count_sem);
        sem_post(&buf_sem[queue]);
        sem_post(&full_sem[queue]);
        sleep(1 + rand() % prod_time);
    }

}

void *consumer() {
    int item, consumer_number;
    for (consumer_number = 0; !pthread_equal(consumers[consumer_number], pthread_self()) &&
                              consumer_number < con_count; consumer_number++) {}
    while (1) {
        sem_wait(&full_sem[consumer_number]);
        sem_wait(&buf_sem[consumer_number]);
        sem_wait(&count_sem);
        item = buf[consumer_number][0];
        for (int i = 0; i < buf_counter[consumer_number]; i++) {
            buf[consumer_number][i] = buf[consumer_number][i + 1];
        }
        printf("Consumer %d consumed %d\n", consumer_number, item);
        --buf_counter[consumer_number];
        print_queues();
        sem_post(&count_sem);
        sem_post(&buf_sem[consumer_number]);
        sem_post(&empty_sem[consumer_number]);
        sleep(1 + rand() % con_time);
    }
}

int main(void) {

    int i;
    srand(time(NULL));
    for (i = 0; i < con_count; i++) {
        sem_init(&buf_sem[i], 0, 1);
        sem_init(&full_sem[i], 0, 0);
        sem_init(&empty_sem[i], 0, buf_len);
    }
    sem_init(&count_sem,0,1);

    for (i = 0; i < prod_count; i++) {

        pthread_create(&producers[i], NULL, &producer, NULL);
    }

    for (i = 0; i < con_count; i++) {

        pthread_create(&consumers[i], NULL, &consumer, NULL);
    }
    for (i = 0; i < prod_count; i++) {
        pthread_join(producers[i], NULL);
    }
    for (i = 0; i < con_count; i++) {
        pthread_join(consumers[i], NULL);
    }
    return 0;
}