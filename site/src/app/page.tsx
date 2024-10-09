"use client";

import React from 'react';
import { useState, useEffect } from 'react';
import { Heading, Text, Flex, Button, Grid, Icon, InlineCode, Logo, Background } from '@/once-ui/components';
import Link from 'next/link';
import { getHostname } from '@/common/utils';

export default function Home() {

	const hostname = getHostname();
	const url = `http://${hostname}:3001`;
	const links = [
		{
			href: "",
			title: "手持终端微内核框架",
			description: "提供面向终端的服务资源管理和内核管理功能.",
		},
		{
			href: "",
			title: "手持终端软件开发模拟器",
			description: "包括模拟、调试和部署工具.",
		},
		{
			href: "",
			title: "手持移动终端远程调试器",
			description: "提供真机和本地端两种调试模式.",
		},
	];

	return (
		<Flex
			fillWidth paddingTop="l" paddingX="l"
			direction="column" alignItems="center" flex={1}>
			<Background lines={false} dots={true}/>
			<Flex
				position="relative"
				as="section" overflow="hidden"
				fillWidth minHeight="0" maxWidth={64}
				direction="column" alignItems="center" flex={1}>
				<Flex
					as="main"
					direction="column" justifyContent="center"
					fillWidth fillHeight padding="l" gap="l">
					<Flex
						mobileDirection="column"
						fillWidth gap="24">
						{/* <Flex
							position="relative"
							fillWidth paddingTop="56" paddingX="xl">
							<Logo size="xl" icon={false} style={{zIndex: '1'}}/>
						</Flex> */}
						<Flex
							position="relative"
							fillWidth gap="24" marginBottom="104"
							direction="column">
				
							<Heading
								variant="display-strong-s">
								手持终端开发调试平台.
							</Heading>
							<Button
								href={url}
								suffixIcon="chevronRight"
								variant="secondary">
								开始使用
							</Button>
						</Flex>
					</Flex>
					<Heading variant="display-strong-xs">
						平台核心模块.
					</Heading>
					<Grid
						radius="l"
						border="neutral-medium"
						borderStyle="solid-1"
						columns="repeat(3, 1fr)"
						tabletColumns="1col"
						mobileColumns="1col"
						fillWidth>
						{links.map((link) => (
							<Link
								target="_blank"
								style={{ padding: 'var(--responsive-space-l)' }}
								key={link.href}
								href={link.href}>
								<Flex
									fillWidth paddingY="8" gap="8"
									direction="column">
									<Flex
										fillWidth gap="12"
										alignItems="center">
										<Text
											variant="body-strong-m" onBackground="neutral-strong">
											{link.title}
										</Text>
										<Icon size="s" name="arrowUpRight" />
									</Flex>
									<Text
										variant="body-default-s" onBackground="neutral-weak">
										{link.description}
									</Text>
								</Flex>
							</Link>
						))}
					</Grid>
				</Flex>
			</Flex>
			<Flex
				as="footer"
				position="relative"
				fillWidth paddingX="l" paddingY="m"
				justifyContent="space-between">
			</Flex>
		</Flex>
	);
}
