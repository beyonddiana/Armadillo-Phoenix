/**
 * @file exoflickr.h
 * @brief Lightweight wrapper around the Flickr API for signing and such
 *
 * $LicenseInfo:firstyear=2012&license=viewerlgpl$
 * Copyright (C) 2012 Katharine Berry
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License only.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * $/LicenseInfo$
 */

#ifndef EXOFLICKR_H
#define EXOFLICKR_H

#include "llimage.h"
#include "llsd.h"

#include <boost/bind.hpp>

// This include file defines the API key/secret.
// It is generated by cmake.
#include "exoflickrkeys.h"

class exoFlickr
{
public:
	typedef boost::function<void(bool success, const LLSD& response)> response_callback_t;

	static void request(const std::string& method, const LLSD& args, response_callback_t callback);
	static void uploadPhoto(const LLSD& args, LLImageFormatted *image, response_callback_t callback);

	static void signRequest(LLSD& params, std::string method, std::string url);
	static std::string getSignatureForCall(const LLSD& parameters, std::string url, std::string method);
};

#endif