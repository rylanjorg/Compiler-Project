
#ifndef CONDITIONAL_BLOCK_TABLE_MANAGER_H
#define CONDITIONAL_BLOCK_TABLE_MANAGER_H

int numLocalConditionalBlocks = 0;

void ResetConditionalBlockCounter()
{
    numLocalConditionalBlocks = 0;
}

void IncrementConditionalBlockCounter()
{
    numLocalConditionalBlocks++;
}

void IncrementConditionalBlockCounterBy(int num)
{
    numLocalConditionalBlocks += num;
}

#endif