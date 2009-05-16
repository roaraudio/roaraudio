#
# spec file for package: [pkg name]
#
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# includes module(s): [pkg module(s)]
#

%include Solaris.inc

Name:		[pkg name]
Summary:      	[short description]
Version:       	[pkg version]
License:	[license type (ie. GPLv2, BSD...)]
Url: 		[url to open source project]
Source:	 	[url to compressed tarball]
Group:		[ips pkg classification]
Distribution:   OpenSolaris
Vendor:		OpenSolaris Community
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
SUNW_Basedir:   %{_basedir}
SUNW_Copyright: %{name}.copyright

%include default-depend.inc

# OpenSolaris IPS Package Manifest Fields
Meta(info.upstream):	 	[name email of open source project leader]
Meta(info.maintainer):	 	[name email of ips pkg porter/maintainer]
Meta(info.repository_url):	[open source code repository]

%description
[long description]

%prep
[pre-build commands]

%build
[build commands]

%install
[install commands]

%clean
[cleanup commands]

%files
[file groups and permissions]

%changelog
[change log entries]
