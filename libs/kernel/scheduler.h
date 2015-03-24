struct task *scheduler_findactive();
struct task *scheduler_findinactive();
void scheduler_attach(struct list *mailboxes);
void scheduler_detach(struct list *mailboxes);
void scheduler_detachall(struct list *mailboxes);
unsigned int scheduler_getactiveid();
unsigned int scheduler_readactive(unsigned int count, void *buffer);
unsigned int scheduler_sendid(unsigned int id, unsigned int count, void *buffer);
void scheduler_sendlist(struct list *mailboxes, unsigned int count, void *buffer);
unsigned int scheduler_readpipe(struct list *mailboxes, unsigned int count, void *buffer);
unsigned int scheduler_sendpipe(struct list *mailboxes, unsigned int count, void *buffer);
void scheduler_use(struct task *task);
void scheduler_unuse(struct task *task);
void scheduler_registertask(struct task *task);
void scheduler_unregistertask(struct task *task);
void scheduler_setup();
