/**
 * \brief Chip-specific PLL definitions.
 *
 */

#ifndef CHIP_PLL_H_INCLUDED
#define CHIP_PLL_H_INCLUDED

#include <osc.h>


/**
 * \weakgroup pll_group
 * @{
 */

#define PLL_OUTPUT_MIN_HZ       160000000
#define PLL_OUTPUT_MAX_HZ       500000000

#define PLL_INPUT_MIN_HZ        3000000
#define PLL_INPUT_MAX_HZ        32000000

#define NR_PLLS             2
#define PLLA_ID             0
#define UPLL_ID             1   //!< USB UTMI PLL.

#define PLL_UPLL_HZ     480000000

#define PLL_COUNT   0x3fU

enum pll_source {
	PLL_SRC_MAINCK_4M_RC   = OSC_MAINCK_4M_RC,  //!< Internal 4MHz RC oscillator.
	PLL_SRC_MAINCK_8M_RC   = OSC_MAINCK_8M_RC,  //!< Internal 8MHz RC oscillator.
	PLL_SRC_MAINCK_12M_RC  = OSC_MAINCK_12M_RC, //!< Internal 12MHz RC oscillator.
	PLL_SRC_MAINCK_XTAL    = OSC_MAINCK_XTAL,   //!< External crystal oscillator.
	PLL_SRC_MAINCK_BYPASS  = OSC_MAINCK_BYPASS, //!< External bypass oscillator.
	PLL_NR_SOURCES,                             //!< Number of PLL sources.
};

struct pll_config {
	uint32_t ctrl;
};

#define pll_get_default_rate(pll_id)                                     \
	((osc_get_rate(CONFIG_PLL##pll_id##_SOURCE)                      \
			* CONFIG_PLL##pll_id##_MUL)                      \
			/ CONFIG_PLL##pll_id##_DIV)

/* Force UTMI PLL parameters (Hardware defined) */
#ifdef CONFIG_PLL1_SOURCE
# undef CONFIG_PLL1_SOURCE
#endif
#ifdef CONFIG_PLL1_MUL
# undef CONFIG_PLL1_MUL
#endif
#ifdef CONFIG_PLL1_DIV
# undef CONFIG_PLL1_DIV
#endif
#define CONFIG_PLL1_SOURCE  PLL_SRC_MAINCK_XTAL
#define CONFIG_PLL1_MUL     0
#define CONFIG_PLL1_DIV     0

/**
 * \note The SAMV71 PLL hardware interprets mul as mul+1. For readability the
 * hardware mul+1 is hidden in this implementation. Use mul as mul effective
 * value.
 */
static inline void pll_config_init(struct pll_config *p_cfg,
		enum pll_source e_src, uint32_t ul_div, uint32_t ul_mul)
{
	uint32_t vco_hz;

	Assert(e_src < PLL_NR_SOURCES);

	if (ul_div == 0 && ul_mul == 0) { /* Must only be true for UTMI PLL */
		p_cfg->ctrl = CKGR_UCKR_UPLLCOUNT(PLL_COUNT);
	} else { /* PLLA */
	/* Calculate internal VCO frequency */
	vco_hz = osc_get_rate(e_src) / ul_div;
	Assert(vco_hz >= PLL_INPUT_MIN_HZ);
	Assert(vco_hz <= PLL_INPUT_MAX_HZ);

	vco_hz *= ul_mul;
	Assert(vco_hz >= PLL_OUTPUT_MIN_HZ);
	Assert(vco_hz <= PLL_OUTPUT_MAX_HZ);

	/* PMC hardware will automatically make it mul+1 */
		p_cfg->ctrl = CKGR_PLLAR_MULA(ul_mul - 1) | CKGR_PLLAR_DIVA(ul_div)  \
		| CKGR_PLLAR_PLLACOUNT(PLL_COUNT);
	}
}

#define pll_config_defaults(cfg, pll_id)                                 \
	pll_config_init(cfg,                                             \
			CONFIG_PLL##pll_id##_SOURCE,                     \
			CONFIG_PLL##pll_id##_DIV,                        \
			CONFIG_PLL##pll_id##_MUL)

static inline void pll_config_read(struct pll_config *p_cfg, uint32_t ul_pll_id)
{
	Assert(ul_pll_id < NR_PLLS);

	if (ul_pll_id == PLLA_ID) {
		p_cfg->ctrl = PMC->CKGR_PLLAR;
	} else {
		p_cfg->ctrl = PMC->CKGR_UCKR;
	}
}

static inline void pll_config_write(const struct pll_config *p_cfg, uint32_t ul_pll_id)
{
	Assert(ul_pll_id < NR_PLLS);

	if (ul_pll_id == PLLA_ID) {
		pmc_disable_pllack(); // Always stop PLL first!
		PMC->CKGR_PLLAR = CKGR_PLLAR_ONE | p_cfg->ctrl;
	} else {
		PMC->CKGR_UCKR = p_cfg->ctrl;
	}
}

static inline void pll_enable(const struct pll_config *p_cfg, uint32_t ul_pll_id)
{
	Assert(ul_pll_id < NR_PLLS);

	if (ul_pll_id == PLLA_ID) {
		pmc_disable_pllack(); // Always stop PLL first!
		PMC->CKGR_PLLAR = CKGR_PLLAR_ONE | p_cfg->ctrl;
	} else {
		PMC->CKGR_UCKR = p_cfg->ctrl | CKGR_UCKR_UPLLEN;
	}
}

/**
 * \note This will only disable the selected PLL, not the underlying oscillator (mainck).
 */
static inline void pll_disable(uint32_t ul_pll_id)
{
	Assert(ul_pll_id < NR_PLLS);

	if (ul_pll_id == PLLA_ID) {
		pmc_disable_pllack();
	} else {
		PMC->CKGR_UCKR &= ~CKGR_UCKR_UPLLEN;
	}
}

static inline uint32_t pll_is_locked(uint32_t ul_pll_id)
{
	Assert(ul_pll_id < NR_PLLS);

	if (ul_pll_id == PLLA_ID) {
	return pmc_is_locked_pllack();
	} else {
		return pmc_is_locked_upll();
	}
}

static inline void pll_enable_source(enum pll_source e_src)
{
	switch (e_src) {
	case PLL_SRC_MAINCK_4M_RC:
	case PLL_SRC_MAINCK_8M_RC:
	case PLL_SRC_MAINCK_12M_RC:
	case PLL_SRC_MAINCK_XTAL:
	case PLL_SRC_MAINCK_BYPASS:
		osc_enable(e_src);
		osc_wait_ready(e_src);
		break;

	default:
		Assert(false);
		break;
	}
}

static inline void pll_enable_config_defaults(unsigned int ul_pll_id)
{
	struct pll_config pllcfg;

	if (pll_is_locked(ul_pll_id)) {
		return; // Pll already running
	}
	switch (ul_pll_id) {
#ifdef CONFIG_PLL0_SOURCE
	case 0:
		pll_enable_source(CONFIG_PLL0_SOURCE);
		pll_config_init(&pllcfg,
				CONFIG_PLL0_SOURCE,
				CONFIG_PLL0_DIV,
				CONFIG_PLL0_MUL);
		break;
#endif
#ifdef CONFIG_PLL1_SOURCE
	case 1:
		pll_enable_source(CONFIG_PLL1_SOURCE);
		pll_config_init(&pllcfg,
				CONFIG_PLL1_SOURCE,
				CONFIG_PLL1_DIV,
				CONFIG_PLL1_MUL);
		break;
#endif
	default:
		Assert(false);
		break;
	}
	pll_enable(&pllcfg, ul_pll_id);
	while (!pll_is_locked(ul_pll_id));
}

/**
 * \ingroup clk_group
 * \defgroup pll_group PLL Management
 *
 * This group contains functions and definitions related to configuring
 * and enabling/disabling on-chip PLLs. A PLL will take an input signal
 * (the \em source), optionally divide the frequency by a configurable
 * \em divider, and then multiply the frequency by a configurable \em
 * multiplier.
 *
 * Some devices don't support input dividers; specifying any other
 * divisor than 1 on these devices will result in an assertion failure.
 * Other devices may have various restrictions to the frequency range of
 * the input and output signals.
 *
 * \par Example: Setting up PLL0 with default parameters
 *
 * The following example shows how to configure and enable PLL0 using
 * the default parameters specified using the configuration symbols
 * listed above.
 * \code
	pll_enable_config_defaults(0); \endcode
 *
 * To configure, enable PLL0 using the default parameters and to disable
 * a specific feature like Wide Bandwidth Mode (a UC3A3-specific
 * PLL option.), you can use this initialization process.
 * \code
	struct pll_config pllcfg;
	if (pll_is_locked(pll_id)) {
		return; // Pll already running
	}
	pll_enable_source(CONFIG_PLL0_SOURCE);
	pll_config_defaults(&pllcfg, 0);
	pll_config_set_option(&pllcfg, PLL_OPT_WBM_DISABLE);
	pll_enable(&pllcfg, 0);
	pll_wait_for_lock(0); \endcode
 *
 * When the last function call returns, PLL0 is ready to be used as the
 * main system clock source.
 *
 * \section pll_group_config Configuration Symbols
 *
 * Each PLL has a set of default parameters determined by the following
 * configuration symbols in the application's configuration file:
 *   - \b CONFIG_PLLn_SOURCE: The default clock source connected to the
 *     input of PLL \a n. Must be one of the values defined by the
 *     #pll_source enum.
 *   - \b CONFIG_PLLn_MUL: The default multiplier (loop divider) of PLL
 *     \a n.
 *   - \b CONFIG_PLLn_DIV: The default input divider of PLL \a n.
 *
 * These configuration symbols determine the result of calling
 * pll_config_defaults() and pll_get_default_rate().
 *
 * @{
 */

//! \name Chip-specific PLL characteristics
//@{
/**
 * \def PLL_MAX_STARTUP_CYCLES
 * \brief Maximum PLL startup time in number of slow clock cycles
 */
/**
 * \def NR_PLLS
 * \brief Number of on-chip PLLs
 */

/**
 * \def PLL_MIN_HZ
 * \brief Minimum frequency that the PLL can generate
 */
/**
 * \def PLL_MAX_HZ
 * \brief Maximum frequency that the PLL can generate
 */
/**
 * \def PLL_NR_OPTIONS
 * \brief Number of PLL option bits
 */
//@}

/**
 * \enum pll_source
 * \brief PLL clock source
 */

//! \name PLL configuration
//@{

/**
 * \struct pll_config
 * \brief Hardware-specific representation of PLL configuration.
 *
 * This structure contains one or more device-specific values
 * representing the current PLL configuration. The contents of this
 * structure is typically different from platform to platform, and the
 * user should not access any fields except through the PLL
 * configuration API.
 */

/**
 * \fn void pll_config_init(struct pll_config *cfg,
 *              enum pll_source src, unsigned int div, unsigned int mul)
 * \brief Initialize PLL configuration from standard parameters.
 *
 * \note This function may be defined inline because it is assumed to be
 * called very few times, and usually with constant parameters. Inlining
 * it will in such cases reduce the code size significantly.
 *
 * \param cfg The PLL configuration to be initialized.
 * \param src The oscillator to be used as input to the PLL.
 * \param div PLL input divider.
 * \param mul PLL loop divider (i.e. multiplier).
 *
 * \return A configuration which will make the PLL run at
 * (\a mul / \a div) times the frequency of \a src
 */
/**
 * \def pll_config_defaults(cfg, pll_id)
 * \brief Initialize PLL configuration using default parameters.
 *
 * After this function returns, \a cfg will contain a configuration
 * which will make the PLL run at (CONFIG_PLLx_MUL / CONFIG_PLLx_DIV)
 * times the frequency of CONFIG_PLLx_SOURCE.
 *
 * \param cfg The PLL configuration to be initialized.
 * \param pll_id Use defaults for this PLL.
 */
/**
 * \def pll_get_default_rate(pll_id)
 * \brief Get the default rate in Hz of \a pll_id
 */
/**
 * \fn void pll_config_set_option(struct pll_config *cfg,
 *              unsigned int option)
 * \brief Set the PLL option bit \a option in the configuration \a cfg.
 *
 * \param cfg The PLL configuration to be changed.
 * \param option The PLL option bit to be set.
 */
/**
 * \fn void pll_config_clear_option(struct pll_config *cfg,
 *              unsigned int option)
 * \brief Clear the PLL option bit \a option in the configuration \a cfg.
 *
 * \param cfg The PLL configuration to be changed.
 * \param option The PLL option bit to be cleared.
 */
/**
 * \fn void pll_config_read(struct pll_config *cfg, unsigned int pll_id)
 * \brief Read the currently active configuration of \a pll_id.
 *
 * \param cfg The configuration object into which to store the currently
 * active configuration.
 * \param pll_id The ID of the PLL to be accessed.
 */
/**
 * \fn void pll_config_write(const struct pll_config *cfg,
 *              unsigned int pll_id)
 * \brief Activate the configuration \a cfg on \a pll_id
 *
 * \param cfg The configuration object representing the PLL
 * configuration to be activated.
 * \param pll_id The ID of the PLL to be updated.
 */

//@}

//! \name Interaction with the PLL hardware
//@{
/**
 * \fn void pll_enable(const struct pll_config *cfg,
 *              unsigned int pll_id)
 * \brief Activate the configuration \a cfg and enable PLL \a pll_id.
 *
 * \param cfg The PLL configuration to be activated.
 * \param pll_id The ID of the PLL to be enabled.
 */
/**
 * \fn void pll_disable(unsigned int pll_id)
 * \brief Disable the PLL identified by \a pll_id.
 *
 * After this function is called, the PLL identified by \a pll_id will
 * be disabled. The PLL configuration stored in hardware may be affected
 * by this, so if the caller needs to restore the same configuration
 * later, it should either do a pll_config_read() before disabling the
 * PLL, or remember the last configuration written to the PLL.
 *
 * \param pll_id The ID of the PLL to be disabled.
 */
/**
 * \fn bool pll_is_locked(unsigned int pll_id)
 * \brief Determine whether the PLL is locked or not.
 *
 * \param pll_id The ID of the PLL to check.
 *
 * \retval true The PLL is locked and ready to use as a clock source
 * \retval false The PLL is not yet locked, or has not been enabled.
 */
/**
 * \fn void pll_enable_source(enum pll_source src)
 * \brief Enable the source of the pll.
 * The source is enabled, if the source is not already running.
 *
 * \param src The ID of the PLL source to enable.
 */
/**
 * \fn void pll_enable_config_defaults(unsigned int pll_id)
 * \brief Enable the pll with the default configuration.
 * PLL is enabled, if the PLL is not already locked.
 *
 * \param pll_id The ID of the PLL to enable.
 */

/**
 * \brief Wait for PLL \a pll_id to become locked
 *
 * \todo Use a timeout to avoid waiting forever and hanging the system
 *
 * \param pll_id The ID of the PLL to wait for.
 *
 * \retval STATUS_OK The PLL is now locked.
 * \retval ERR_TIMEOUT Timed out waiting for PLL to become locked.
 */
static inline int pll_wait_for_lock(unsigned int pll_id)
{
	Assert(pll_id < NR_PLLS);

	while (!pll_is_locked(pll_id)) {
		/* Do nothing */
	}

	return 0;
}

#endif /* CHIP_PLL_H_INCLUDED */
