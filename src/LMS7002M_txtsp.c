///
/// \file LMS7002M_txtsp.c
///
/// TX DSP chain config for the LMS7002M C driver.
///
/// \copyright
/// Copyright (c) 2014-2015 Fairwaves, Inc.
/// Copyright (c) 2014-2015 Rice University
/// SPDX-License-Identifier: Apache-2.0
/// http://www.apache.org/licenses/LICENSE-2.0
///

#include <stdlib.h>
#include "LMS7002M_impl.h"

LMS7002M_API void LMS7002M_txtsp_enable(LMS7002M_t *self, const LMS7002M_chan_t channel, const bool enable)
{
    LMS7002M_set_mac_ch(self, channel);

    self->regs.reg_0x0200_en = enable?1:0;
    self->regs.reg_0x0200_bstart = 0;
    self->regs.reg_0x0200_insel = REG_0X0200_INSEL_LML;
    LMS7002M_regs_spi_write(self, 0x0200);

    self->regs.reg_0x0203_hbi_ovr = REG_0X0203_HBI_OVR_BYPASS;

    self->regs.reg_0x0208_cmix_byp = 1;
    self->regs.reg_0x0208_isinc_byp = 1;
    self->regs.reg_0x0208_gfir3_byp = 1;
    self->regs.reg_0x0208_gfir2_byp = 1;
    self->regs.reg_0x0208_gfir1_byp = 1;
    self->regs.reg_0x0208_dc_byp = 1;
    self->regs.reg_0x0208_gc_byp = 1;
    self->regs.reg_0x0208_ph_byp = 1;

    LMS7002M_regs_spi_write(self, 0x0200);
    LMS7002M_regs_spi_write(self, 0x0203);
    LMS7002M_regs_spi_write(self, 0x0208);
}

LMS7002M_API void LMS7002M_txtsp_set_interp(LMS7002M_t *self, const LMS7002M_chan_t channel, const size_t interp)
{
    LMS7002M_set_mac_ch(self, channel);

    if (interp == 1) self->regs.reg_0x0203_hbi_ovr = REG_0X0203_HBI_OVR_BYPASS;
    if (interp == 2) self->regs.reg_0x0203_hbi_ovr = 0;
    if (interp == 4) self->regs.reg_0x0203_hbi_ovr = 1;
    if (interp == 8) self->regs.reg_0x0203_hbi_ovr = 2;
    if (interp == 16) self->regs.reg_0x0203_hbi_ovr = 3;
    if (interp == 32) self->regs.reg_0x0203_hbi_ovr = 4;

    LMS7002M_regs_spi_write(self, 0x0203);
}

LMS7002M_API void LMS7002M_txtsp_set_freq(LMS7002M_t *self, const LMS7002M_chan_t channel, const double freqRel)
{
    LMS7002M_set_mac_ch(self, channel);
    self->regs.reg_0x0208_cmix_byp = 0;
    LMS7002M_regs_spi_write(self, 0x0208);
    LMS7002M_set_nco_freq(self, LMS_TX, channel, freqRel);
}

LMS7002M_API void LMS7002M_txtsp_tsg_const(LMS7002M_t *self, const LMS7002M_chan_t channel, const int valI, const int valQ)
{
    LMS7002M_set_mac_ch(self, channel);

    //muxes
    self->regs.reg_0x0200_tsgfc = REG_0X0200_TSGFC_FS;
    self->regs.reg_0x0200_tsgmode = REG_0X0200_TSGMODE_DC;
    self->regs.reg_0x0200_insel = REG_0X0200_INSEL_TEST;
    LMS7002M_regs_spi_write(self, 0x0200);

    //load I
    self->regs.reg_0x020c_dc_reg = valI;
    LMS7002M_regs_spi_write(self, 0x020c);
    self->regs.reg_0x0200_tsgdcldi = 0;
    LMS7002M_regs_spi_write(self, 0x0200);
    self->regs.reg_0x0200_tsgdcldi = 1;
    LMS7002M_regs_spi_write(self, 0x0200);
    self->regs.reg_0x0200_tsgdcldi = 0;
    LMS7002M_regs_spi_write(self, 0x0200);

    //load Q
    self->regs.reg_0x020c_dc_reg = valQ;
    LMS7002M_regs_spi_write(self, 0x020c);
    self->regs.reg_0x0200_tsgdcldq = 0;
    LMS7002M_regs_spi_write(self, 0x0200);
    self->regs.reg_0x0200_tsgdcldq = 1;
    LMS7002M_regs_spi_write(self, 0x0200);
    self->regs.reg_0x0200_tsgdcldq = 0;
    LMS7002M_regs_spi_write(self, 0x0200);
}

LMS7002M_API void LMS7002M_txtsp_tsg_tone(LMS7002M_t *self, const LMS7002M_chan_t channel)
{
    LMS7002M_set_mac_ch(self, channel);

    //muxes
    self->regs.reg_0x0200_tsgmode = REG_0X0200_TSGMODE_NCO;
    self->regs.reg_0x0200_insel = REG_0X0200_INSEL_TEST;
    LMS7002M_regs_spi_write(self, 0x0200);

    self->regs.reg_0x0200_tsgfcw = REG_0X0200_TSGFCW_DIV8;
    LMS7002M_regs_spi_write(self, 0x0200);
}