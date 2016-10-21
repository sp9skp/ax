/*
 * Functions for controlling ax radios
 * Copyright (C) 2016  Richard Meadows <richardeoin>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef AX_H
#define AX_H

#include <stdlib.h>
#include <stdint.h>

/**
 * Initialisation Status
 */
typedef enum ax_init_status {
  AX_INIT_OK = 0,
  AX_INIT_PORT_FAILED,
  AX_INIT_BAD_SCRATCH,
  AX_INIT_BAD_REVISION,
  AX_INIT_SET_SPI,
} ax_init_status;

/**
 * Represents the chosen modulation scheme.
 */
typedef struct ax_modulation {
  uint8_t modulation;           /* modulation */
  uint8_t encoding;             /* encoding */
  uint8_t framing;              /* framing */
  uint32_t bitrate;             /* link bitrate provided to user */
  uint8_t fec;                  /* 0 = no fec, 1 = fec enabled */

  float power;                  /* TX output power */

  union {
    struct {                    /* FSK */
      float modulation_index;
    } fsk;
    struct {                    /* AFSK */
      uint16_t deviation;       /* (Hz) */
      uint16_t space, mark;     /* (Hz) */
    } afsk;
  } parameters;

  uint32_t max_delta_carrier;   /* max. delta from carrier centre, autoset if 0 */
  /* larger increases the time for the AFC to achieve lock */

  uint8_t decimation;           /* set automatically */
  uint32_t rxdatarate;          /* set automatically */

} ax_modulation;

/**
 * CONFIG ----------------------------------------------------------------------
 */

/* Clock source type */
enum ax_clock_source_type {
  AX_CLOCK_SOURCE_CRYSTAL,
  AX_CLOCK_SOURCE_TCXO,
};
/* VCO type - See Datasheet Table 8. */
enum ax_vco_type {
  AX_VCO_INTERNAL = 0,
  AX_VCO_INTERNAL_EXTERNAL_INDUCTOR,
  AX_VCO_EXTERNAL,
};
/* Divider at the output of the VCO  */
enum ax_rfdiv {
  AX_RFDIV_UKNOWN = 0,
  AX_RFDIV_0,
  AX_RFDIV_1,
};


/**
 * Represents one of the two physical synthesisers.
 */
typedef struct ax_synthesiser {
  uint32_t frequency;
  uint32_t register_value;
  enum ax_rfdiv rfdiv;     /* set if this is known, else it's set automatically */
  uint8_t vco_range_known; /* set to 0 if vco range unknown */
  uint8_t vco_range;       /* determined by autoranging */
} ax_synthesiser;


/**
 * configuration
 */
typedef struct ax_config {
  /* power mode */
  uint8_t pwrmode;

  /* synthesiser */
  struct {
    ax_synthesiser A, B;
    enum ax_vco_type vco_type;  /* default is internal */
  } synthesiser;

  /* external clock */
  enum ax_clock_source_type clock_source; /* Crystal or TCXO */
  uint32_t f_xtal;              /* external clock frequency (Hz) */
  uint16_t load_capacitance;    /* if crystal, load capacitance to be applied (pF) */
  uint32_t error_ppm;           /* max. error of clock source, ppm */
  uint8_t f_xtaldiv;            /* xtal division factor, set automatically */
  void* (*tcxo_enable)(void);    /* function to enable tcxo */
  void* (*tcxo_disable)(void);   /* function to disable tcxo */

  /* spi transfer */
  void* (*spi_transfer)(unsigned char*, uint8_t);

  /* receive */
  void* (*rx_callback)(unsigned char*, uint8_t);
  uint8_t pkt_store_flags;      /* PKTSTOREFLAGS */

  /* pll vco */
  uint32_t f_pllrng;

  /* rx parameters */


} ax_config;


/**
 * FUNCTION PROTOTYPES ---------------------------------------------------------
 */

/* transmit */
void ax_tx_on(ax_config* config, ax_modulation* mod);
void ax_tx_packet(ax_config* config, uint8_t* packet, uint16_t length);

/* receive */
void ax_rx_on(ax_config* config, ax_modulation* mod);

/* init */
int ax_init(ax_config* config);

#endif  /* AX_H */