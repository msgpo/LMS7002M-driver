///
/// \file LMS7002M/LMS7002M_cgen.h
///
/// Clock generation for the LMS7002M C driver.
///
/// \copyright
/// Copyright (c) 2014-2015 Fairwaves, Inc.
/// Copyright (c) 2014-2015 Rice University
/// SPDX-License-Identifier: Apache-2.0
/// http://www.apache.org/licenses/LICENSE-2.0
///

#pragma once
#include <stdlib.h>
#include <LMS7002M/LMS7002M.h>

#ifdef __cplusplus
extern "C" {
#endif

LMS7002M_API int LMS7002M_set_data_clock(LMS7002M_t *self, const double fref, const double fout)
{
    //The equations:
    // fref * N = fvco
    // fvco / fdiv = fout
    // fref * N = fout * fdiv
    int fdiv = 2;
    double Ndiv = 0;
    double fvco = 0;

    //calculation loop to find dividers that are possible
    while (true)
    {
        Ndiv = fout*fdiv/fref;
        fvco = fout*fdiv;

        //check dividers and vco in range...
        if (fdiv > 512) return -1;
        if (Ndiv < 4) continue;
        if (Ndiv > 512) return -1;
        //these boundaries observed from the EVB7 GUI
        if (fvco < 2.6e9) continue;
        if (fvco > 2.7e9) continue;

        //failed to find a good value, try the next even divider
        fdiv += 2;
    }

    //configure and enable synthesizer
    self->regs.reg_0x0086_en_intonly_sdm_cgen = 0; //support frac-N
    self->regs.reg_0x0086_en_sdm_clk_cgen = 1; //enable
    self->regs.reg_0x0086_pd_cp_cgen = 0; //enable
    self->regs.reg_0x0086_pd_fdiv_fb_cgen = 0; //enable
    self->regs.reg_0x0086_pd_fdiv_o_cgen = 0; //enable
    self->regs.reg_0x0086_pd_sdm_cgen = 0; //enable
    self->regs.reg_0x0086_pd_vco_cgen = 0; //enable
    self->regs.reg_0x0086_pd_vco_comp_cgen = 0; //enable

    //program the N divider
    const int Nint = (int)Ndiv;
    const int Nfrac = (int)((Ndiv-Nint)*(1 << 20));
    self->regs.reg_0x0087_frac_sdm_cgen = (Nfrac-1) & 0xffff; //lower 16 bits
    self->regs.reg_0x0088_frac_sdm_cgen = (Nfrac-1) >> 16; //upper 4 bits
    self->regs.reg_0x0088_int_sdm_cgen = Nint-1;

    //program the feedback divider
    self->regs.reg_0x0089_sel_sdmclk_cgen = REG_0X0089_SEL_SDMCLK_CGEN_CLK_DIV;
    self->regs.reg_0x0089_div_outch_cgen = (fdiv/2)-1;

    //write affected registers
    LMS7002M_regs_spi_write(self, 0x0086);
    LMS7002M_regs_spi_write(self, 0x0087);
    LMS7002M_regs_spi_write(self, 0x0088);
    LMS7002M_regs_spi_write(self, 0x0089);

    return 0; //OK
}

#ifdef __cplusplus
}
#endif
