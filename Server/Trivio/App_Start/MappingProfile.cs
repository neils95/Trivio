using AutoMapper;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using Trivio.Models;
using Trivio.Models.DTOs;

namespace Trivio.App_Start
{
	public class MappingProfile :Profile
	{
		public MappingProfile()
		{
			CreateMap<UserRegistrationDTO, User>();
			CreateMap<User, UserPublicDTO>();
			CreateMap<UserTriviaHistory,UserTriviaHistoryDTO>()
				.ForMember(dest=> dest.Trivia, opt=>opt.MapFrom(src=>src.Trivia));


		}
	}
}