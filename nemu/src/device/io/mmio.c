#include "common.h"
#include "device/map.h"

#define NR_MAP 8

static IOMap maps[NR_MAP] = {};
static int nr_map = 0;

/* 内存映射I/O, 但内存映射I/O的读写并不是面向CPU的，为什么？ */
/* device interface */

void add_mmio_map(char *name, paddr_t addr, uint8_t* space, int len, io_callback_t callback) {
  assert(nr_map < NR_MAP);
  maps[nr_map] = (IOMap){ .name = name,
                          .low = addr, 
                          .high = addr + len - 1,
                          .space = space, 
                          .callback = callback };

  Log("Add mmio map '%s' at [0x%08x, 0x%08x]", maps[nr_map].name, maps[nr_map].low, maps[nr_map].high);

  nr_map ++;
}

/* bus interface 
 * 遍历maps, 如果某一项的地址映射包含addr, find_mapid_by_addr返回其id
 */
IOMap* fetch_mmio_map(paddr_t addr) {
  int mapid = find_mapid_by_addr(maps, nr_map, addr);
  return (mapid == -1 ? NULL : &maps[mapid]);
}
