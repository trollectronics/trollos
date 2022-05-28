#ifndef __MMU040_H__
#define __MMU040_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <mmu.h>

typedef enum Mmu040PageSize Mmu040PageSize;
enum Mmu040PageSize {
	MMU040_PAGE_SIZE_4K,
	MMU040_PAGE_SIZE_8K,
};

typedef enum Mmu040CacheMode Mmu040CacheMode;
enum Mmu040CacheMode {
	MMU040_CACHE_MODE_CACHE_WRITE_THROUGH,
	MMU040_CACHE_MODE_CACHE_COPY_BACK,
	MMU040_CACHE_MODE_NOCACHE_SERIALIZE,
	MMU040_CACHE_MODE_NOCACHE,
};

typedef enum Mmu040SField Mmu040SField;
enum Mmu040SField {
	MMU040_S_FIELD_MATCH_USER,
	MMU040_S_FIELD_MATCH_SUPERVISOR,
	MMU040_S_FIELD_MATCH_BOTH,
};

typedef enum Mmu040PageDescriptorType Mmu040PageDescriptorType;
enum Mmu040PageDescriptorType {
	MMU040_PAGE_DESCRIPTOR_TYPE_INVALID,
	MMU040_PAGE_DESCRIPTOR_TYPE_RESIDENT,
	MMU040_PAGE_DESCRIPTOR_TYPE_INDIRECT,
	MMU040_PAGE_DESCRIPTOR_TYPE_RESIDENT_ALT,
};

typedef enum Mmu040UpperLevelDescriptorType Mmu040UpperLevelDescriptorType;
enum Mmu040UpperLevelDescriptorType {
	MMU040_UPPER_LEVEL_DESCRIPTOR_TYPE_INVALID,
	MMU040_UPPER_LEVEL_DESCRIPTOR_TYPE_INVALID_ALT,
	MMU040_UPPER_LEVEL_DESCRIPTOR_TYPE_RESIDENT,
	MMU040_UPPER_LEVEL_DESCRIPTOR_TYPE_RESIDENT_ALT,
};

typedef struct Mmu040RegTranslationControl Mmu040RegTranslationControl;
struct Mmu040RegTranslationControl {
	uint16_t enable : 1;
	uint16_t page_size : 1;
	uint16_t : 14;
};

typedef struct Mmu040RegTransparentTranslation Mmu040RegTransparentTranslation;
struct Mmu040RegTransparentTranslation {
	uint32_t logical_address_base : 8;
	uint32_t logical_address_mask : 8;
	uint32_t enable : 1;
	uint32_t s_field : 2;
	uint32_t : 3;
	uint32_t user_page_attributes : 2;
	uint32_t : 1;
	uint32_t cache_mode : 2;
	uint32_t : 2;
	uint32_t write_protected : 1;
	uint32_t : 2;
};

typedef struct Mmu040RegStatus Mmu040RegStatus;
struct Mmu040RegStatus {
	uint32_t physical_address : 20;
	uint32_t bus_error : 1;
	uint32_t global : 1;
	uint32_t user_page_attributes : 2;
	uint32_t supervisor : 1;
	uint32_t cache_mode : 2;
	uint32_t modified : 1;
	uint32_t write_protected : 1;
	uint32_t transparent_translation_hit : 1;
	uint32_t resident : 1;
};

typedef struct Mmu040RegRootPointer Mmu040RegRootPointer;
struct Mmu040RegRootPointer {
	uint32_t root_pointer : 23;
	uint32_t : 9;
};

typedef union Mmu040RootTableDescriptor Mmu040RootTableDescriptor;
union Mmu040RootTableDescriptor {
	struct {
		uint32_t table_address : 23;
		uint32_t : 5;
		uint32_t used : 1;
		uint32_t write_protected : 1;
		uint32_t upper_level_descriptor_type : 2;
	} table;
	uint32_t whole;
};

typedef union Mmu040PointerTableDescriptor Mmu040PointerTableDescriptor;
union Mmu040PointerTableDescriptor {
	struct {
		uint32_t table_address : 25;
		uint32_t : 3;
		uint32_t used : 1;
		uint32_t write_protected : 1;
		uint32_t upper_level_descriptor_type : 2;
	} table;
	uint32_t whole;
};

typedef struct Mmu040PageTableDescriptorPage Mmu040PageTableDescriptorPage;
struct Mmu040PageTableDescriptorPage {
	uint32_t physical_address : 20;
	uint32_t user_reserved : 1;
	uint32_t global : 1;
	uint32_t user_page_attributes : 2;
	uint32_t supervisor : 1;
	uint32_t cache_mode : 2;
	uint32_t modified : 1;
	uint32_t used : 1;
	uint32_t write_protected : 1;
	uint32_t page_descriptor_type : 2;
};

typedef struct Mmu040PageTableDescriptorIndirect Mmu040PageTableDescriptorIndirect;
struct Mmu040PageTableDescriptorIndirect {
	uint32_t descriptor_address : 30;
	uint32_t page_descriptor_type : 2;
};

typedef union Mmu040PageTableDescriptor Mmu040PageTableDescriptor;
union Mmu040PageTableDescriptor {
	Mmu040PageTableDescriptorPage page;
	Mmu040PageTableDescriptorIndirect indirect;
	uint32_t whole;
	
};

void mmu040_init();
void *mmu040_allocate_frame(uint32_t virtual_address, bool write_protected);
void mmu_bus_error();
uint32_t mmu040_test_read(void *addr);
void *mmu040_get_physical_manual(uint32_t virtual_address);
PhysicalAddress mmu040_get_physical(void *phys, bool supervisor);
void mmu040_invalidate_page(void *addr);
void mmu040_print_status();

int mmu040_init_userspace(MmuUserspaceHandle *userspace);
void mmu040_clear_userspace(MmuUserspaceHandle *userspace);
void mmu040_free_userspace(MmuUserspaceHandle *userspace);
int mmu040_clone_userspace(MmuUserspaceHandle *from, MmuUserspaceHandle *to);
int mmu040_switch_userspace(MmuUserspaceHandle *userspace);

PhysicalAddress mmu040_alloc_at(void *virt, bool supervisor, bool write_protected);
void mmu040_free_at(void *virt, bool supervisor);

void mmu040_fill_frame(PhysicalAddress frame, int offset, void *src, unsigned int size);
void mmu040_copy_from_userspace(void *dst, void *src, size_t size);
void mmu040_copy_to_userspace(void *dst, void *src, size_t size);
void mmu040_map_current_userspace();

void mmu040_zero_4k(void *addr);
void mmu040_set_tc(Mmu040RegTranslationControl *tc);
void mmu040_get_tc(Mmu040RegTranslationControl *tc);
void mmu040_set_srp(Mmu040RegRootPointer *srp);
void mmu040_get_srp(Mmu040RegRootPointer *srp);
void mmu040_set_urp(Mmu040RegRootPointer *urp);
void mmu040_get_urp(Mmu040RegRootPointer *urp);
void mmu040_set_dttr(Mmu040RegTransparentTranslation *ttr, int num);
void mmu040_get_dttr(Mmu040RegTransparentTranslation *ttr, int num);
void mmu040_set_ittr(Mmu040RegTransparentTranslation *ttr, int num);
void mmu040_get_ittr(Mmu040RegTransparentTranslation *ttr, int num);


#endif
