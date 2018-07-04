#include "nand_flash_manager.h"
#include <KernelMPI.h>
#include <DebugMPI.h>

LF_USING_BRIO_NAMESPACE()

extern CDebugMPI debug;
static CKernelMPI kernel;

nand_flash_manager::nand_flash_manager()
{
}

nand_flash_manager::~nand_flash_manager()
{
}

int nand_flash_manager::flash()
{
    debug.DebugOut(kDbgLvlVerbose,"%s IN\n", __PRETTY_FUNCTION__);    
    static int ret = 0;
    kernel.TaskSleep(10000);
    ret = 1 - ret;
    return ret;
}

int nand_flash_manager::verify()
{
    debug.DebugOut(kDbgLvlVerbose,"%s IN\n", __PRETTY_FUNCTION__);    
    static int ret = 0;
    kernel.TaskSleep(10000);
    ret = 1 - ret;
    return ret;
}
