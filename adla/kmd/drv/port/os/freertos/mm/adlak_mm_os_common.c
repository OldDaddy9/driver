/*******************************************************************************
 * Copyright (C) 2022 Amlogic, Inc. All rights reserved.
 ******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file adlak_mm_os_common.c
 * @brief
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   	Who				Date				Changes
 * ----------------------------------------------------------------------------
 * 1.00a shiwei.sun@amlogic.com	2022/07/15	Initial release
 * </pre>
 *
 ******************************************************************************/

/***************************** Include Files *********************************/
#include "adlak_mm_os_common.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

#if (CONFIG_ADLAK_MEM_POOL_EN && defined(CONFIG_ADLAK_USE_RESERVED_MEMORY))

void adlak_unmap_region_nocache(struct adlak_mem *mm) {
    if (ADLAK_IS_ERR_OR_NULL(mm->mem_pool)) {
        return;
    }
    adlak_os_free(mm->mem_pool);
    mm->mem_pool = NULL;
}

int adlak_remap_region_nocache(struct adlak_mem *mm) {
    phys_addr_t physical;
    size_t      size;
    void *      vaddr = NULL;

    physical = mm->padlak->hw_res.adlak_resmem_pa;
    size     = mm->padlak->hw_res.adlak_resmem_size;
    if (0 == size) {
        goto err;
    }
    /**Assuming the physical and virtual addresses are the same in freertos**/
    vaddr = (void *)(uintptr_t)physical;

    if (!vaddr) {
        AML_LOG_ERR("fail to map physical to kernel space\n");
        vaddr = NULL;
        goto err;
    }

    mm->mem_pool = adlak_os_zalloc(sizeof(struct adlak_mem_pool_info), ADLAK_GFP_KERNEL);
    if (ADLAK_IS_ERR_OR_NULL(mm->mem_pool)) {
        goto err_alloc;
    }

    mm->mem_pool->cpu_addr_base  = vaddr;
    mm->mem_pool->phys_addr_base = physical;
    mm->mem_pool->dma_addr_base  = physical;
    mm->mem_pool->size           = size;
    mm->mem_pool->mem_src        = ADLAK_ENUM_MEMSRC_RESERVED;
    mm->mem_pool->cacheable      = false;

    AML_LOG_INFO("Reserved memory info: dma_addr= 0x%lX,  phys_addr= 0x%lX,size=%lu MByte\n",
                 (uintptr_t)mm->mem_pool->dma_addr_base, (uintptr_t)mm->mem_pool->phys_addr_base,
                 (uintptr_t)(mm->mem_pool->size / (1024 * 1024)));

    return 0;
err_alloc:

err:
    return -1;
}

#endif

static int adlak_flush_cache_init(struct adlak_mem *mm, struct adlak_mem_handle *mm_info,
                                  uint32_t offset, uint32_t size) {
    return ERR(NONE);
}

static int adlak_flush_cache_destroy(struct adlak_mem *mm, struct adlak_mem_handle *mm_info) {
    return ERR(NONE);
}

void adlak_os_free_discontiguous(struct adlak_mem *mm, struct adlak_mem_handle *mm_info) {
    return adlak_os_free_contiguous(mm, mm_info);
}

int adlak_os_alloc_discontiguous(struct adlak_mem *mm, struct adlak_mem_handle *mm_info) {
    return adlak_os_alloc_contiguous(mm, mm_info);
}
static phys_addr_t adlak_get_phys_addr(void *vaddr) { return (phys_addr_t)vaddr; }

void adlak_os_free_contiguous(struct adlak_mem *mm, struct adlak_mem_handle *mm_info) {
    AML_LOG_DEBUG("%s", __func__);

    if (mm_info->cpu_addr) {
        adlak_os_free(mm_info->cpu_addr);
        adlak_os_free(mm_info->phys_addrs);
    }
}

int adlak_os_alloc_contiguous(struct adlak_mem *mm, struct adlak_mem_handle *mm_info) {
    void *       vaddr_start = NULL;
    void *       cpu_addr    = NULL;
    phys_addr_t *phys_addrs  = NULL;
    int          i, order;
    size_t       size = mm_info->req.bytes;

    AML_LOG_DEBUG("%s", __func__);

    phys_addrs =
        (phys_addr_t *)adlak_os_zalloc(sizeof(phys_addr_t) * mm_info->nr_pages, ADLAK_GFP_KERNEL);
    if (!phys_addrs) {
        goto err_alloc_phys_addrs;
    }
    mm_info->phys_addrs = phys_addrs;

    vaddr_start = adlak_os_malloc(size, 0);
    if (!vaddr_start) {
        AML_LOG_ERR("malloc %d bytes failed", size);
        goto err_malloc_vaddr;
    }
    AML_LOG_DEBUG("vaddr_start=0x%lX", (uintptr_t)vaddr_start);

    for (i = 0; i < mm_info->nr_pages; ++i) {
        phys_addrs[i] =
            adlak_get_phys_addr(vaddr_start + ADLAK_PAGE_SIZE * i);  // get physical addr
    }

    mm_info->cpu_addr = vaddr_start;

    AML_LOG_DEBUG("%s: PA=0x%llx,VA_kernel=0x%lX", __FUNCTION__, (uint64_t)phys_addrs[0],
                  (uintptr_t)cpu_addr);

    mm_info->phys_addr = phys_addrs[0];

    return ERR(NONE);
err_malloc_vaddr:
    adlak_os_free(phys_addrs);
err_alloc_phys_addrs:

    return ERR(ENOMEM);
}

int adlak_os_attach_ext_mem_phys(struct adlak_mem *mm, struct adlak_mem_handle *mm_info,
                                 uint64_t phys_addr) {
    phys_addr_t *phys_addrs = NULL;
    int          i;
    AML_LOG_DEBUG("%s", __func__);
    mm_info->phys_addrs = NULL;
    phys_addrs =
        (phys_addr_t *)adlak_os_zalloc(sizeof(phys_addr_t) * mm_info->nr_pages, ADLAK_GFP_KERNEL);
    if (!phys_addrs) {
        goto err_alloc_phys_addrs;
    }

    mm_info->phys_addr = phys_addr;
    for (i = 0; i < mm_info->nr_pages; ++i) {
        phys_addrs[i] = phys_addr + (i * ADLAK_PAGE_SIZE);  // get physical addr
        AML_LOG_DEBUG("phys_addrs[%d]=0x%llx", i, (uint64_t)(uintptr_t)phys_addrs[i]);
    }

    mm_info->pages      = NULL;
    mm_info->phys_addrs = phys_addrs;
    mm_info->cpu_addr   = NULL;
    mm_info->dma_addr   = (dma_addr_t)NULL;

    return ERR(NONE);
err_alloc_phys_addrs:

    return ERR(ENOMEM);
}

void adlak_os_dettach_ext_mem(struct adlak_mem *mm, struct adlak_mem_handle *mm_info) {
    AML_LOG_DEBUG("%s", __func__);
    if (mm_info->phys_addrs) {
        adlak_os_free(mm_info->phys_addrs);
    }
}

int adlak_os_mmap(struct adlak_mem *mm, struct adlak_mem_handle *mm_info, void *const vma) {
    /* @brief  Not required in freertos     */

    return 0;
}

void adlak_os_flush_cache(struct adlak_mem *mm, struct adlak_mem_handle *mm_info,
                          struct adlak_sync_cache_ext_info *sync_cache_ext_info) {
    struct sg_table *sgt = NULL;
    AML_LOG_DEBUG("%s", __func__);
    if (!(mm_info->mem_type &
          (ADLAK_ENUM_MEMTYPE_INNER_USER_CACHEABLE | ADLAK_ENUM_MEMTYPE_INNER_KERNEL_CACHEABLE))) {
        return;
    }
    {
        // TODO(user):
        // aml_osal_cache_clean(mm_info->dma_addr, mm_info->req.bytes);
    }
}

void adlak_os_invalid_cache(struct adlak_mem *mm, struct adlak_mem_handle *mm_info,
                            struct adlak_sync_cache_ext_info *sync_cache_ext_info) {
    struct sg_table *sgt = NULL;
    AML_LOG_DEBUG("%s", __func__);
    if (!(mm_info->mem_type &
          (ADLAK_ENUM_MEMTYPE_INNER_USER_CACHEABLE | ADLAK_ENUM_MEMTYPE_INNER_KERNEL_CACHEABLE))) {
        return;
    }
    {
        // TODO(user):
        // aml_osal_cache_invalidate(mm_info->dma_addr, mm_info->req.bytes);
    }
}

void adlak_free_share_through_dma(struct adlak_mem *mm, struct adlak_mem_handle *mm_info) {
    // TODO - implement adlak_free_share_through_dma
    AML_LOG_ERR("Not yet implemented");
    ASSERT(0);
}

int adlak_malloc_share_through_dma(struct adlak_mem *mm, struct adlak_mem_handle *mm_info) {
    // TODO - implement adlak_malloc_share_through_dma
    AML_LOG_ERR("Not yet implemented");
    ASSERT(0);
}

void adlak_free_through_dma(struct adlak_mem *mm, struct adlak_mem_handle *mm_info) {
    return adlak_os_free_contiguous(mm, mm_info);
}

int adlak_malloc_through_dma(struct adlak_mem *mm, struct adlak_mem_handle *mm_info) {
    return adlak_os_alloc_contiguous(mm, mm_info);
}

int adlak_os_mmap2userspace(struct adlak_mem *mm, struct adlak_mem_handle *mm_info) {
    mm_info->cpu_addr_user = mm_info->cpu_addr;
    return 0;
}

void adlak_os_unmmap_userspace(struct adlak_mem *mm, struct adlak_mem_handle *mm_info) {
    mm_info->cpu_addr_user = 0;
}

void *adlak_os_mm_vmap(struct adlak_mem_handle *mm_info) { return mm_info->cpu_addr; }

void adlak_os_mm_vunmap(struct adlak_mem_handle *mm_info) {}