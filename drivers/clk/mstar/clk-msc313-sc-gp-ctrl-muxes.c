// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Daniel Palmer
 */

#include <linux/clk-provider.h>
#include <linux/mfd/syscon.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>

#include "clk-msc313-mux.h"

static const struct clk_parent_data emac_rxtx_parents[] = {
	{ .fw_name = "eth_buf" },
	{ .fw_name = "rmii_buf" },
};

static const struct clk_parent_data emac_rxtx_ref_parents[] = {
	{ .fw_name = "rmii_buf" },
};

#define EMAC_MUXES \
	MSC313_MUX_DATA("emac_rx", emac_rxtx_parents, 0x88, 0, 2, 1, -1), \
	MSC313_MUX_DATA("emac_rx_ref", emac_rxtx_ref_parents, 0x88, 8, 10, 1, -1), \
	MSC313_MUX_DATA("emac_tx", emac_rxtx_parents, 0x8c, 0, 2, 1, -1), \
	MSC313_MUX_DATA("emac_tx_ref", emac_rxtx_ref_parents, 0x8c, 8, 10, 1, -1)

static const struct msc313_mux_data msc313_muxes[] = {
	EMAC_MUXES,
};

static const struct msc313_muxes_data msc313_data = MSC313_MUXES_DATA(msc313_muxes);

static const struct msc313_mux_data ssd20xd_muxes[] = {
	EMAC_MUXES,
	MSC313_MUX_DATA("emac1_rx", emac_rxtx_parents, 0xcc, 0, 2, 1, -1),
	MSC313_MUX_DATA("emac1_rx_ref", emac_rxtx_ref_parents, 0xcc, 8, 10, 1, -1),
	MSC313_MUX_DATA("emac1_tx", emac_rxtx_parents, 0xd0, 0, 2, 1, -1),
	MSC313_MUX_DATA("emac1_tx_ref", emac_rxtx_ref_parents, 0xd0, 8, 10, 1, -1),
};

static const struct msc313_muxes_data ssd20xd_data = MSC313_MUXES_DATA(ssd20xd_muxes);

static const struct of_device_id msc313e_sc_gp_ctrl_muxes_of_match[] = {
	{
		.compatible = "mstar,msc313-sc-gp-ctrl-muxes",
		.data = &msc313_data,
	},
	{
		.compatible = "sstar,ssd20xd-sc-gp-ctrl-muxes",
		.data = &ssd20xd_data,
	},
	{}
};

static int msc313e_clkgen_mux_probe(struct platform_device *pdev)
{
	const struct msc313_muxes_data *match_data;
	struct device *dev = &pdev->dev;
	struct regmap *regmap;

	printk("probe\n");

	match_data = of_device_get_match_data(dev);
	if (!match_data)
		return -EINVAL;

	regmap = syscon_node_to_regmap(dev->parent->of_node);
	if (IS_ERR(regmap))
		return PTR_ERR(regmap);

	return msc313_mux_register_muxes(dev, regmap, match_data);
}

static struct platform_driver msc313_sc_gp_ctrl_muxes_driver = {
	.driver = {
		.name = "msc313-sc-gp-ctrl-muxes",
		.of_match_table = msc313e_sc_gp_ctrl_muxes_of_match,
	},
	.probe = msc313e_clkgen_mux_probe,
};
builtin_platform_driver(msc313_sc_gp_ctrl_muxes_driver);
