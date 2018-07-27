/**
 * \brief Chip-specific generic clock management.
 */

#ifndef CHIP_GENCLK_H_INCLUDED
#define CHIP_GENCLK_H_INCLUDED

#include <osc.h>
#include <pll.h>


/**
 * \weakgroup genclk_group
 * @{
 */

//! \name Programmable Clock Identifiers (PCK)
//@{
#define GENCLK_PCK_0 0 //!< PCK0 ID
#define GENCLK_PCK_1 1 //!< PCK1 ID
#define GENCLK_PCK_2 2 //!< PCK2 ID
//@}

//! \name Programmable Clock Sources (PCK)
//@{

enum genclk_source {
	GENCLK_PCK_SRC_SLCK_RC       = 0,//!< Internal 32kHz RC oscillator as PCK source clock
	GENCLK_PCK_SRC_SLCK_XTAL     = 1,//!< External 32kHz crystal oscillator as PCK source clock
	GENCLK_PCK_SRC_SLCK_BYPASS   = 2,//!< External 32kHz bypass oscillator as PCK source clock
	GENCLK_PCK_SRC_MAINCK_4M_RC  = 3,//!< Internal 4MHz RC oscillator as PCK source clock
	GENCLK_PCK_SRC_MAINCK_8M_RC  = 4,//!< Internal 8MHz RC oscillator as PCK source clock
	GENCLK_PCK_SRC_MAINCK_12M_RC = 5,//!< Internal 12MHz RC oscillator as PCK source clock
	GENCLK_PCK_SRC_MAINCK_XTAL   = 6,//!< External crystal oscillator as PCK source clock
	GENCLK_PCK_SRC_MAINCK_BYPASS = 7,//!< External bypass oscillator as PCK source clock
	GENCLK_PCK_SRC_PLLACK        = 8,//!< Use PLLACK as PCK source clock
	GENCLK_PCK_SRC_MCK           = 9,//!< Use Master Clk as PCK source clock
};

//@}

//! \name Programmable Clock Prescalers (PCK)
//@{

enum genclk_divider {
	GENCLK_PCK_PRES_1  = PMC_PCK_PRES(0), //!< Set PCK clock prescaler to 1
	GENCLK_PCK_PRES_2  = PMC_PCK_PRES(1), //!< Set PCK clock prescaler to 2
	GENCLK_PCK_PRES_4  = PMC_PCK_PRES(2), //!< Set PCK clock prescaler to 4
	GENCLK_PCK_PRES_8  = PMC_PCK_PRES(3), //!< Set PCK clock prescaler to 8
	GENCLK_PCK_PRES_16 = PMC_PCK_PRES(4), //!< Set PCK clock prescaler to 16
	GENCLK_PCK_PRES_32 = PMC_PCK_PRES(5), //!< Set PCK clock prescaler to 32
	GENCLK_PCK_PRES_64 = PMC_PCK_PRES(6), //!< Set PCK clock prescaler to 64
};

//@}

struct genclk_config {
	uint32_t ctrl;
};

static inline void genclk_config_defaults(struct genclk_config *p_cfg,
		uint32_t ul_id)
{
	ul_id = ul_id;
	p_cfg->ctrl = 0;
}

static inline void genclk_config_read(struct genclk_config *p_cfg,
		uint32_t ul_id)
{
	p_cfg->ctrl = PMC->PMC_PCK[ul_id];
}

static inline void genclk_config_write(const struct genclk_config *p_cfg,
		uint32_t ul_id)
{
	PMC->PMC_PCK[ul_id] = p_cfg->ctrl;
}

//! \name Programmable Clock Source and Prescaler configuration
//@{

static inline void genclk_config_set_source(struct genclk_config *p_cfg,
		enum genclk_source e_src)
{
	p_cfg->ctrl &= (~PMC_PCK_CSS_Msk);

	switch (e_src) {
	case GENCLK_PCK_SRC_SLCK_RC:
	case GENCLK_PCK_SRC_SLCK_XTAL:
	case GENCLK_PCK_SRC_SLCK_BYPASS:
		p_cfg->ctrl |= (PMC_PCK_CSS_SLOW_CLK);
		break;

	case GENCLK_PCK_SRC_MAINCK_4M_RC:
	case GENCLK_PCK_SRC_MAINCK_8M_RC:
	case GENCLK_PCK_SRC_MAINCK_12M_RC:
	case GENCLK_PCK_SRC_MAINCK_XTAL:
	case GENCLK_PCK_SRC_MAINCK_BYPASS:
		p_cfg->ctrl |= (PMC_PCK_CSS_MAIN_CLK);
		break;

	case GENCLK_PCK_SRC_PLLACK:
		p_cfg->ctrl |= (PMC_PCK_CSS_PLLA_CLK);
		break;

	case GENCLK_PCK_SRC_MCK:
		p_cfg->ctrl |= (PMC_PCK_CSS_MCK);
		break;

	default:
		break;
	}
}

static inline void genclk_config_set_divider(struct genclk_config *p_cfg,
		uint32_t e_divider)
{
	p_cfg->ctrl &= ~PMC_PCK_PRES_Msk;
	p_cfg->ctrl |= e_divider;
}

//@}

static inline void genclk_enable(const struct genclk_config *p_cfg, uint32_t ul_id)
{
	PMC->PMC_PCK[ul_id] = p_cfg->ctrl;
	pmc_enable_pck(ul_id);
}

static inline void genclk_disable(uint32_t ul_id)
{
	pmc_disable_pck(ul_id);
}

static inline void genclk_enable_source(enum genclk_source e_src)
{
	switch (e_src) {
	case GENCLK_PCK_SRC_SLCK_RC:
		if (!osc_is_ready(OSC_SLCK_32K_RC)) {
			osc_enable(OSC_SLCK_32K_RC);
			osc_wait_ready(OSC_SLCK_32K_RC);
		}
		break;

	case GENCLK_PCK_SRC_SLCK_XTAL:
		if (!osc_is_ready(OSC_SLCK_32K_XTAL)) {
			osc_enable(OSC_SLCK_32K_XTAL);
			osc_wait_ready(OSC_SLCK_32K_XTAL);
		}
		break;

	case GENCLK_PCK_SRC_SLCK_BYPASS:
		if (!osc_is_ready(OSC_SLCK_32K_BYPASS)) {
			osc_enable(OSC_SLCK_32K_BYPASS);
			osc_wait_ready(OSC_SLCK_32K_BYPASS);
		}
		break;

	case GENCLK_PCK_SRC_MAINCK_4M_RC:
		if (!osc_is_ready(OSC_MAINCK_4M_RC)) {
			osc_enable(OSC_MAINCK_4M_RC);
			osc_wait_ready(OSC_MAINCK_4M_RC);
		}
		break;

	case GENCLK_PCK_SRC_MAINCK_8M_RC:
		if (!osc_is_ready(OSC_MAINCK_8M_RC)) {
			osc_enable(OSC_MAINCK_8M_RC);
			osc_wait_ready(OSC_MAINCK_8M_RC);
		}
		break;

	case GENCLK_PCK_SRC_MAINCK_12M_RC:
		if (!osc_is_ready(OSC_MAINCK_12M_RC)) {
			osc_enable(OSC_MAINCK_12M_RC);
			osc_wait_ready(OSC_MAINCK_12M_RC);
		}
		break;

	case GENCLK_PCK_SRC_MAINCK_XTAL:
		if (!osc_is_ready(OSC_MAINCK_XTAL)) {
			osc_enable(OSC_MAINCK_XTAL);
			osc_wait_ready(OSC_MAINCK_XTAL);
		}
		break;

	case GENCLK_PCK_SRC_MAINCK_BYPASS:
		if (!osc_is_ready(OSC_MAINCK_BYPASS)) {
			osc_enable(OSC_MAINCK_BYPASS);
			osc_wait_ready(OSC_MAINCK_BYPASS);
		}
		break;

#ifdef CONFIG_PLL0_SOURCE
	case GENCLK_PCK_SRC_PLLACK:
		pll_enable_config_defaults(0);
		break;
#endif

	case GENCLK_PCK_SRC_MCK:
		break;

	default:
		Assert(false);
		break;
	}
}

/**
 * \ingroup clk_group
 * \defgroup genclk_group Generic Clock Management
 *
 * Generic clocks are configurable clocks which run outside the system
 * clock domain. They are often connected to peripherals which have an
 * asynchronous component running independently of the bus clock, e.g.
 * USB controllers, low-power timers and RTCs, etc.
 *
 * Note that not all platforms have support for generic clocks; on such
 * platforms, this API will not be available.
 *
 * @{
 */

/**
 * \def GENCLK_DIV_MAX
 * \brief Maximum divider supported by the generic clock implementation
 */
/**
 * \enum genclk_source
 * \brief Generic clock source ID
 *
 * Each generic clock may be generated from a different clock source.
 * These are the available alternatives provided by the chip.
 */

//! \name Generic clock configuration
//@{
/**
 * \struct genclk_config
 * \brief Hardware representation of a set of generic clock parameters
 */
/**
 * \fn void genclk_config_defaults(struct genclk_config *cfg,
 *              unsigned int id)
 * \brief Initialize \a cfg to the default configuration for the clock
 * identified by \a id.
 */
/**
 * \fn void genclk_config_read(struct genclk_config *cfg, unsigned int id)
 * \brief Read the currently active configuration of the clock
 * identified by \a id into \a cfg.
 */
/**
 * \fn void genclk_config_write(const struct genclk_config *cfg,
 *              unsigned int id)
 * \brief Activate the configuration \a cfg on the clock identified by
 * \a id.
 */
/**
 * \fn void genclk_config_set_source(struct genclk_config *cfg,
 *              enum genclk_source src)
 * \brief Select a new source clock \a src in configuration \a cfg.
 */
/**
 * \fn void genclk_config_set_divider(struct genclk_config *cfg,
 *              unsigned int divider)
 * \brief Set a new \a divider in configuration \a cfg.
 */
/**
 * \fn void genclk_enable_source(enum genclk_source src)
 * \brief Enable the source clock \a src used by a generic clock.
 */
 //@}

//! \name Enabling and disabling Generic Clocks
//@{
/**
 * \fn void genclk_enable(const struct genclk_config *cfg, unsigned int id)
 * \brief Activate the configuration \a cfg on the clock identified by
 * \a id and enable it.
 */
/**
 * \fn void genclk_disable(unsigned int id)
 * \brief Disable the generic clock identified by \a id.
 */
//@}

/**
 * \brief Enable the configuration defined by \a src and \a divider
 * for the generic clock identified by \a id.
 *
 * \param id      The ID of the generic clock.
 * \param src     The source clock of the generic clock.
 * \param divider The divider used to generate the generic clock.
 */
static inline void genclk_enable_config(unsigned int id, enum genclk_source src, unsigned int divider)
{
	struct genclk_config gcfg;

	genclk_config_defaults(&gcfg, id);
	genclk_enable_source(src);
	genclk_config_set_source(&gcfg, src);
	genclk_config_set_divider(&gcfg, divider);
	genclk_enable(&gcfg, id);
}

#endif /* CHIP_GENCLK_H_INCLUDED */
