#
#  Copyright (2015) Intel Corporation All Rights Reserved.
#
#  This software is supplied under the terms of a license
#  agreement or nondisclosure agreement with Intel Corp.
#  and may not be copied or disclosed except in accordance
#  with the terms of that agreement.
#

define make_spec

%{!?kversion: %define kversion %(uname -r)}
%{!?kernel_src: %define kernel_src /lib/modules/%{kversion}/build}
%{!?kernel_base_version:  %define kernel_base_version  %(echo %kversion | awk -F '-' '{print $$1}')}
%define kernel_stock_build %(echo %kversion | grep '.el7.centos.' | grep '.x86_64')
%if "%{kernel_stock_build}" != ""
%{!?kernel_extra_version: %define kernel_extra_version %(echo %kversion | awk -F '-' '{print $$2}' | sed 's/.x86_64//')}
%else
%{!?kernel_extra_version: %define kernel_extra_version %(echo %kversion | awk -F '-' '{print $$2}')}
%endif

Summary:  Host drivers for Intel® VCA cards
Name: $(name)
Version: %{_version}
Release: $(release)
License: GPLv2
Group: kernel
Vendor: Intel Corporation
URL: http://www.intel.com
Source0: %{name}-%{version}.tar.gz
%if %{_vendor} == suse
BuildRequires: kernel-headers kernel-default-devel
%else
BuildRequires: kernel-headers kernel-devel
%endif
%description
Provides host drivers for Intel® VCA cards

%if "%{kernel_stock_build}" != ""
%define rpmname $(name)-%{kernel_base_version}-%{kernel_extra_version}
%define rpmdevelname $(name)-devel-%{kernel_base_version}-%{kernel_extra_version}
%else
%define rpmname $(name)-%{kversion}
%define rpmdevelname $(name)-devel-%{kversion}
%endif
%package -n %{rpmname}
Group: kernel
Summary: Host driver for Intel® VCA cards

# Centos 6 and 7 kernel makefiles differ in the way kernel RPM
# version is derived from kernel version (uname -r). We have a
# dependency on the kernel version, so need to report this differently
# based on target OS distribution.
Requires: gawk
%if "%{dist}" == ".el7.centos" || "%{dist}" == ".el7"
# Centos 7 kernel version differs depending on kernel build type
# If kernel was built from complete sources, it contains '_'
# If kernel was built by applying custom patches to stock kernel sources, it contains '-'
%if "%{kernel_stock_build}" != ""
Requires: kernel = %{kernel_base_version}-%{kernel_extra_version}
%else
Requires: kernel = %{kernel_base_version}_%{kernel_extra_version}
%endif
%else
%if "%{dist}" == ".el6"
Requires: kernel = %{kernel_base_version}%{kernel_extra_version}
%else
ERROR unknown %{dist}
%endif
%endif

%description -n %{rpmname}
Provides host driver for Intel® VCA cards

%package -n %{rpmdevelname}
Group:   kernel
Summary: Header and symbol version file for driver development
%description -n %{rpmdevelname}
Provides header and symbol version file for driver development

# Don't include %{kversion} in headers. It's simpler for others
# who depend on module-headers
%package -n $(name)-headers
Group:   kernel
Summary: Development header files specific to Intel VCA
%description -n $(name)-headers
Development header files specific to Intel VCA

%prep
%setup -D -q -c -T -a 0

%build
$(make_prefix)%{__make} VCA_CARD_ARCH=k1om KERNEL_SRC=%{kernel_src} \
	KERNEL_VERSION=%{kversion} VCASS_BUILDNO=%{version} $(make_postfix)

%install
%{__make} KERNEL_VERSION=%{kversion} KERNEL_SRC=%{kernel_src} \
	kmodincludedir=/usr/src/kernels/%{kversion}/include/modules \
	DESTDIR=%{buildroot} VCA_CARD_ARCH=k1om prefix="" install \
	VCASS_BUILDNO=%{version}
%{__make} KERNEL_VERSION=%{kversion} KERNEL_SRC=%{kernel_src} \
	kmodincludedir=/usr/src/kernels/%{kversion}/include/modules \
	DESTDIR=%{buildroot}/usr VCA_CARD_ARCH=k1om prefix="" dev_install
rm -f %{buildroot}/lib/modules/%{kversion}/modules.*
$(extra_install)

%clean

%post -n %{rpmname}
/sbin/depmod -a %{kversion}
if [ $$1 == 1 ]; then
	/sbin/vca_setup.sh
fi

%preun -n %{rpmname}
if [ $$1 == 0 ]; then
	if [ -e /sbin/vca_uninstall.sh ]; then
		/sbin/vca_uninstall.sh
	fi
fi

%postun -n %{rpmname}
/sbin/depmod -a %{kversion}

%files -n %{rpmname}
%defattr(-,root,root,-)
/etc/modprobe.d/
/lib/modules/%{kversion}/extra/plx87xx.ko
/lib/modules/%{kversion}/extra/vca/plx87xx_dma/plx87xx_dma.ko
/lib/modules/%{kversion}/extra/vca/bus/vop_bus.ko
/lib/modules/%{kversion}/extra/vca/vop/vop.ko
/lib/modules/%{kversion}/extra/vca/bus/vca_csm_bus.ko
/lib/modules/%{kversion}/extra/vca/vca_csm/vca_csm.ko
/lib/modules/%{kversion}/extra/vca/bus/vca_mgr_bus.ko
/lib/modules/%{kversion}/extra/vca/vca_mgr/vca_mgr.ko
/lib/modules/%{kversion}/extra/vca/bus/vca_mgr_extd_bus.ko
/lib/modules/%{kversion}/extra/vca/vca_mgr_extd/vca_mgr_extd.ko
/lib/modules/%{kversion}/extra/vca/bus/vca_csa_bus.ko
/lib/modules/%{kversion}/extra/vca/vca_csa/vca_csa.ko
/lib/modules/%{kversion}/extra/vca/blockio/vcablk_bckend.ko
/lib/modules/%{kversion}/extra/vca/blockio/vcablkfe.ko
/lib/modules/%{kversion}/extra/vca/vca_virtio/vca_virtio.ko
/lib/modules/%{kversion}/extra/vca/vca_virtio/vca_virtio_net.ko
/lib/modules/%{kversion}/extra/vca/vca_virtio/vca_virtio_ring.ko
/lib/modules/%{kversion}/extra/vca/vca_virtio/vca_vringh.ko

/etc/vca_config
$(extra_files)
%defattr(700, root, root, -)
/sbin/vca_eth_cfg.sh
/sbin/vca_setup.sh
/sbin/vca_uninstall.sh
/sbin/vca_agent.sh
/sbin/vca_elog.py
/lib/systemd/system/vca_agent.service

%files -n %{rpmdevelname}
%defattr (-,root,root)
/lib/modules/%{kversion}/scif.symvers

%files -n $(name)-headers
%defattr(-,-,-,-)
"/usr/include/vca_dev_common.h"
"/usr/include/vca_common.h"
"/usr/include/vca_ioctl.h"
"/usr/include/vca_mgr_ioctl.h"
"/usr/include/vca_mgr_extd_ioctl.h"
"/usr/include/vca_csm_ioctl.h"

%changelog

endef

export make_spec
