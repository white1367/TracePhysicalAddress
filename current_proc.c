#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <asm/page.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>

static pid_t pid_1 = 1;
static pid_t pid_2 = 2;

module_param(pid_1, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
module_param(pid_2, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

static unsigned long vaddr2paddr(struct task_struct *task,unsigned long vaddr){
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    unsigned long paddr = 0;
    unsigned long page_addr = 0;
    unsigned long page_offset = 0;

    pgd = pgd_offset(task->mm, vaddr);
    printk("pgd_val = 0x%lx\n",pgd_val(*pgd));
    printk("pgd_index = %lu\n", pgd_index(vaddr));
    if(pgd_none(*pgd)){
        printk("not mapped in pdg\n");
        return -1;
    }
    
    p4d = p4d_offset(pgd, vaddr);
    printk("p4d_val = 0x%lx\n", p4d_val(*p4d));
    printk("p4d_index = %lu\n", p4d_index(vaddr));
    if (p4d_none(*p4d)) {
        printk("not mapped in p4d\n");
        return -1;
    }

    pud = pud_offset(p4d, vaddr);
    printk("pud_val = 0x%lx\n", pud_val(*pud));
    printk("pud_index = %lu\n", pud_index(vaddr));
    if (pud_none(*pud)) {
        printk("not mapped in pud\n");
        return -1;
    }

    pmd = pmd_offset(pud, vaddr);
    printk("pmd_val = 0x%lx\n", pmd_val(*pmd));
    printk("pmd_index = %lu\n", pmd_index(vaddr));
    if (pmd_none(*pmd)) {
        printk("not mapped in pmd\n");
        return -1;
    }

    pte = pte_offset_kernel(pmd, vaddr);
    printk("pte_val = 0x%lx\n", pte_val(*pte));
    printk("pte_index = %lu\n", pte_index(vaddr));
    if (pte_none(*pte)) {
        printk("not mapped in pte\n");
        return -1;
    }

    /* Page frame physical address mechanism | offset */
    page_addr = pte_val(*pte) & PAGE_MASK;
    page_offset = vaddr & ~PAGE_MASK;
    paddr = page_addr | page_offset;
    printk("page_addr = %lx, page_offset = %lx\n", page_addr, page_offset);
    printk("vaddr = %lx, paddr = %lx\n", vaddr, paddr);

    return paddr;

}

int char_arr_init (void) {
    unsigned long vaddr = 0;
    struct task_struct *p1 = pid_task(find_get_pid(pid_1), PIDTYPE_PID);
    struct task_struct *p2 = pid_task(find_get_pid(pid_2), PIDTYPE_PID);
    printk(KERN_INFO "This first process is \"%s\" (pid %i)\n", p1-> comm, p1->pid);
    printk(KERN_INFO "This second process is \"%s\" (pid %i)\n", p2-> comm, p2->pid);
    vaddr = (unsigned long)vmalloc(1000 * sizeof(char));
    printk("vmalloc_vaddr=0x%lx\n", vaddr);
    printk("----------Start of %d----------\n",pid_1);
    vaddr2paddr(p1, vaddr);
    printk("----------End of %d----------\n",pid_1);
    printk("----------Start of %d----------\n",pid_2);
    vaddr2paddr(p2, vaddr);
    printk("----------End of %d----------\n",pid_2);
    return 0;
}

void char_arr_cleanup(void) {
    printk(KERN_INFO " Inside cleanup_module\n");
    remove_proc_entry("process_data",NULL);
}
MODULE_LICENSE("GPL");   
module_init(char_arr_init);
module_exit(char_arr_cleanup);
