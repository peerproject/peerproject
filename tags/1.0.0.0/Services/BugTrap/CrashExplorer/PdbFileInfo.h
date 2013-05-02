/*
 * This is a part of the BugTrap package.
 * Copyright (c) 2005-2009 IntelleSoft.
 * All rights reserved.
 *
 * Description: Source file information.
 * Author: Maksim Pyatkovskiy.
 */

#pragma once

#include "BaseFileInfo.h"

/// Source file information.
class CPdbFileInfo : public CBaseFileInfo
{
public:
	/// Initialize empty object.
	CPdbFileInfo() { }
	/**
	 * @brief Initialize the object.
	 * @param strFileName - source file name.
	 */
	CPdbFileInfo(const std::string& strFileName) : m_strFileName(strFileName) { }
	/**
	 * @brief Get source file name.
	 * @return source file name.
	 */
	virtual const std::string& GetFileName() const { return m_strFileName; }

private:
	/// Source file name.
	std::string m_strFileName;
};
